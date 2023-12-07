#include <algorithm>
#include <chrono>
#include <deque>
#include <numeric>

#include "graph.cpp"

namespace MMC
{
	bool Graph::checkcycle(NodeId v, bool visited, bool *instack)
	{
		if(instack[v])
		 return true;
		if(visited[v])
		 return false;
		visited[v]=true;
		instack[v]=true;
		for(NodeId v: node(v).outgoing_halfedges())   
		{
			if(checkcycle(v,visited,*instack))
			 return true;
		}
		instack[v]=false;
		
		return true;
	}
	bool Graph::checkcycle1()
	{
		bool visited;
		std::vector<bool> instack;
		for(NodeId v=0 ; v<num_nodes(); v++)
		 if(checkcycle(v, visited, *instack))
		    return true;
		return false;    
	}
	
	void Graph::Minimumcycle()
	{
		std::vector<Node> p;
		if(checkcyle1())
		 std::cout<<"Graph is acyclic";
		else
		{
			Edgeweight gamma= *max_element(_edge_weights.begin(),_edge_weights.end());
			for(EdgeId e; e<num_edges();e++)
			{
				edge_weight(e)=edge_weight(e)-1;
			}
			for (NodeId v; v<num_nodes(); v++)
			{
			p=Shortestpath(v, _outgoing_halfedges,weight);
					
			}
			
		
    	}
	}
	
	void Graph::Shortestpath(NodeId v)
	{ 
	  std::vector<EdgeWeight> l;
	  a= std::numeric_limits<double>::infinity();
	  std::vector<Node> R;
	  std::vector<Node> p;
	  std::vector<NodeId> v
	  l(v)=0;
	  for(NodeId s=0 ; s<num_nodes(); s++)
	  {
	  	if(s!=v)
	  	l(s)=a;
	  
	  
	  }
	  R.push_back(v);
	  for(NodeId w; w <num_nodes(); w++)
	  {
	  	if (!std::find(R.begin(),R.end(),w) && node(w).outgoing_halfedges(v))
	  	{
	  		if(l(w)>l(v))+halfedge_weight(v)+halfedge_weight(w))
	  		{
	  			l(w)=l(v)+halfedge_weight(v)+halfedge_weight(w)
	  			p(v)=w;
			  }
		  }
	  }
	  
	  return p;
	  	
	}
}
