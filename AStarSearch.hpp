/*
A* Algorithm Implementation using STL is
Copyright (C)2001-2005 Justin Heyes-Jones

A* Algorithm Implementation not using STL is
Copyright (C)2008 Konstantin Stupnik
Optimized version of original implementation.


Permission is given by the author to freely redistribute and
include this code in any program as long as this credit is
given where due.

  COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
  INCLUDING, WITHOUT LIMITATION, WARRANTIES THAT THE COVERED CODE
  IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
  OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND
  PERFORMANCE OF THE COVERED CODE IS WITH YOU. SHOULD ANY COVERED
  CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT THE INITIAL
  DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY
  NECESSARY SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF
  WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS LICENSE. NO USE
  OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
  THIS DISCLAIMER.

  Use at your own risk!

*/

#ifndef __ASTARSEARCH_HPP__
#define __ASTARSEARCH_HPP__

#include "IntrSortedList.hpp"
#include <sys/types.h>
#include <unordered_map>
#include "MemPool.hpp"
#ifdef DEBUGASTAR
#include <stdio.h>
#define asdebugprintf(x) printf x
#else
#define asdebugprintf(x)
#endif

template <class UserState,class CostType>
struct StateCostTmpl{
  UserState state;
  CostType cost;
  StateCostTmpl(){}
  StateCostTmpl(const UserState& argState,CostType argCost):state(argState),cost(argCost){}
};


template <class MapClass,class UserState,typename CostType=int,int maxSuccessors=8>
class AStarSearch
{
public: // data

  enum AStarSearchState
  {
    SEARCH_STATE_NOT_INITIALISED,
    SEARCH_STATE_SEARCHING,
    SEARCH_STATE_SUCCEEDED,
    SEARCH_STATE_FAILED,
    SEARCH_STATE_OUT_OF_MEMORY,
    SEARCH_STATE_INVALID
  };


  // constructor just initialises private data
  AStarSearch() :
    userMap(0),
    nodeMap(0,UserStateHash(this)),
    successorsSize(0),
    state( SEARCH_STATE_NOT_INITIALISED ),
    steps(0),
    start(0),
    goal(0),
    currentSolutionNode( NULL ),
    cancelRequest( false )
  {
  }

  ~AStarSearch()
  {
  }

  void assignMap(MapClass* argUserMap)
  {
    userMap=argUserMap;
  }


  // call at any time to cancel the search
  void cancelSearch()
  {
    cancelRequest = true;
  }

  // Set Start and goal states
  void setStartAndGoalStates( const UserState &argStart, const UserState &argGoal )
  {

    asdebugprintf(("start=(%s), end=(%s)\n",argStart.toString().c_str(),argGoal.toString().c_str()));

    nodeMap.clear();
    mpool.reset();
    // Clear open list after previous searches
    openList.Clear();

    cancelRequest = false;


    // allocate start end goal nodes
    start = getNode(argStart);
    goal = getNode(argGoal);

    start->userState = argStart;
    goal->userState = argGoal;

    //goal is the same as start
    //do not need to do anything
    if(argStart==argGoal)
    {
      state = SEARCH_STATE_SUCCEEDED;
      start->child=0;
      return;
    }
    else
    {
      state = SEARCH_STATE_SEARCHING;
    }

    // Initialise the AStar specific parts of the Start Node
    // The user only needs fill out the state information

    start->g = 0;
    start->h = userMap->goalDistanceEstimate(start->userState,goal->userState );
    start->f = start->g + start->h;
    start->parent = 0;

    // Push the start node on the Open list

    start->na=naOpen;
    openList.Push( start );

    asdebugprintf(("push:%d:%s\n",start->f,start->userState.toString().c_str()));

    // Initialise counter for search steps
    steps = 0;
  }

  // Advances search one step
  AStarSearchState searchStep()
  {
    // Firstly break if the user has not initialised the search

    if( state != SEARCH_STATE_SEARCHING )
    {
      return state;
    }

    // Failure is defined as emptying the open list as there is nothing left to
    // search...
    // New: Allow user abort
    if( openList.isEmpty() || cancelRequest )
    {
      state = SEARCH_STATE_FAILED;
      return state;
    }

    // Incremement step count
    steps ++;

    // Pop the best node (the one with the lowest f)
    Node *n= openList.Pop();

    asdebugprintf(("Pop:%d:%s\n",n->f,n->userState.toString().c_str()));

    // Check for the goal, once we pop that we're done
    //if( n->userState.isEqual( &goal->userState ) )
    if( n==goal )
    {
      // The user is going to use the Goal Node he passed in
      // so copy the parent pointer of n
      //goal->parent = n->parent;

      // set the child pointers in each node (except Goal which has no child)
      Node *nodeChild = goal;
      Node *nodeParent = goal->parent;
      do
      {
        nodeParent->child = nodeChild;
        nodeChild = nodeParent;
        nodeParent = nodeParent->parent;
      }
      while( nodeChild != start ); // Start is always the first node by definition

      state = SEARCH_STATE_SUCCEEDED;

      return state;
    }
    else // not goal
    {

      // We now need to generate the successors of this node
      // The user helps us to do this, and we keep the new nodes in
      // successors ...

      // User provides this functions and uses AddSuccessor to add each successor of
      // node 'n' to successors

      successorsSize=userMap->getSuccessors( successors, &n->userState,n->parent?&n->parent->userState:0);

      StateCost* it=successors;
      StateCost* end=successors+successorsSize;

      //Node tempNode;
      Node* successor;
      CostType newg;
      bool previousFound;

      // Now handle each successor to the current node ...
      for( ; it != end; it++ )
      {
#ifdef DEBUGASTAR
        if(userMap->getIndex(it->state)>=nodesMapSize)
        {
          asdebugprintf(("State index is out of range:%s(%d)\n",it->state.toString().c_str(),(int)userMap->getIndex(it->state)));
          state=SEARCH_STATE_OUT_OF_MEMORY;
          return state;
        }
#endif

        successor=getNode(it->state);

        //  The g value for this successor ...
        newg = n->g + it->cost;

        asdebugprintf(("successor: g=%d, %s\n",newg,it->state.toString().c_str()));

        // Now we need to find whether the node is on the open or closed lists
        // If it is but the node that is already on them is better (lower g)
        // then we can forget about this successor
        previousFound=successor->na!=naNone;

        if( previousFound )
        {
          asdebugprintf(("found in previous: %s, g=%d\n",(successor->na==naOpen)?"open":((successor->na==naClosed)?"closed":"none"),successor->g));

          if(successor->na==naOpen && successor->g <= newg)
          {
            // we found this state on open
            // the one on Open is cheaper than this one
            continue;
          }
          if(successor->na==naClosed && successor->g <= newg)
          {
            // the one on Closed is cheaper than this one
            continue;
          }
        }else
        {
          successor->child=0;
          successor->userState=it->state;
        }

        // This node is the best node so far with this particular state
        // so lets keep it and set up its AStar specific data ...

        successor->parent = n;
        successor->g = newg;
        successor->h = userMap->goalDistanceEstimate(successor->userState,goal->userState );
        successor->f = successor->g + successor->h;

        //previous version of this state found in open list,
        //but this one is better. let's update it.
        if(previousFound && successor->na==naOpen)
        {
          openList.Update( successor );
          asdebugprintf(("update:%d\n",(int)openList.getSize()));
        }else
        {
          successor->na=naOpen;
          openList.Push( successor );
          asdebugprintf(("push:%d,%d\n",successor->f,(int)openList.getSize()));
        }
      }

      // mark n as Closed, as we have expanded it now

      n->na=naClosed;

    } // end else (not goal so expand)

    return state; // state is searching at this point

  }

  // Functions for traversing the solution

  // Get start node
  UserState *getSolutionStart()
  {
    currentSolutionNode = start;
    if( start )
    {
      return &start->userState;
    }
    else
    {
      return NULL;
    }
  }

  // Get next node
  UserState *getSolutionNext()
  {
    if( currentSolutionNode )
    {
      if( currentSolutionNode->child )
      {

        currentSolutionNode = currentSolutionNode->child;

        return &currentSolutionNode->userState;
      }
    }

    return NULL;
  }

  // Get end node
  UserState *getSolutionEnd()
  {
    currentSolutionNode = goal;
    if( goal )
    {
      return &goal->userState;
    }
    else
    {
      return NULL;
    }
  }

  // Step solution iterator backwards
  UserState *getSolutionPrev()
  {
    if( currentSolutionNode )
    {
      if( currentSolutionNode->parent )
      {

        currentSolutionNode = currentSolutionNode->parent;

        return &currentSolutionNode->userState;
      }
    }

    return NULL;
  }

  // Get the number of steps

  int getStepCount()const
  {
    return steps;
  }

private: // methods

  typedef StateCostTmpl<UserState,CostType> StateCost;
  // A node represents a possible state in the search
  // The user provided state type is included inside this type

  enum NodeAffilation{
    naNone,
    naClosed,
    naOpen
  };

  struct Node:IntrSortedListNodeBase<Node>
  {
      Node *parent; // used during the search to record the parent of successor nodes
      Node *child; // used after the search for the application to view the search in reverse

      CostType g; // cost of this node + it's predecessors
      CostType h; // heuristic estimate of distance to goal
      CostType f; // sum of cumulative cost of predecessors and self and heuristic

      NodeAffilation na;


      bool lessThan(const Node* rhs)const
      {
        return f < rhs->f;
      }

      UserState userState;
      void init()
      {
        na=naNone;
        parent=0;
        child=0;
      }
  };


  //private copy constructor
  //class cannot be copied
  AStarSearch(const AStarSearch&);

  struct UserStateHash{
    AStarSearch* self;
    UserStateHash(AStarSearch* argSelf):self(argSelf){}
    uint32_t operator()(const UserState& state)const
    {
      return self->userMap->getHash(state);
    }
  };

  //use provided class that represent access to map
  MapClass* userMap;
  typedef std::unordered_map<UserState,Node*,UserStateHash> NodeMap;
  NodeMap nodeMap;
  MemPool<128,Node> mpool;

  Node* getNode(const UserState& pos)
  {
    Node* rv=0;
    typename NodeMap::iterator it=nodeMap.find(pos);
    if(it==nodeMap.end())
    {
      rv=mpool.alloc();
      rv->init();
      nodeMap.insert(typename NodeMap::value_type(pos,rv));
    }else
    {
      rv=it->second;
    }
    return rv;
  }

  //sorted list for open nodes
  typedef IntrSortedList<Node> OpenNodeContainer;
  OpenNodeContainer openList;

  //current node to add successors to
  StateCost successors[maxSuccessors];
  size_t successorsSize;

  // State
  AStarSearchState state;

  // Counts steps
  int steps;

  // Start and goal state pointers
  Node *start;
  Node *goal;
  Node *currentSolutionNode;

  bool cancelRequest;

};

#undef asdebugprintf

#endif
