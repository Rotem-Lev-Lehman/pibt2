#pragma once
#include "solver.hpp"

class PP_MAPFSolver: public MAPF_Solver
{
public:
    static const std::string SOLVER_NAME;
protected:
    int k;  // k - minimal revealed privacy per agent (the amount of optional nodes we might be at each timestamp)

private:

  struct SubAgent {
    int id;
    Node *start;
    Node *goal;
  };

  using SubAgents = std::vector<SubAgent*>;

  // PP_MAPFSolver agent
  struct Agent {
    int id;
    int real_sub_agent_id;
    SubAgents *sub_agents;
  };
  using Agents = std::vector<Agent*>;



private:
    Agent * generate_agent(int id);
    void generate_configs(Agents& agents, Config& start, Config& goal);
    void set_solution(Agents& agents, Plan& solution);
    Node * generate_random_node();

protected:
    void run();

public:
    PP_MAPFSolver(MAPF_Instance* _P);
    ~PP_MAPFSolver() {}

    void setParams(int argc, char* argv[]);
    static void printHelp();

    int getK() const { return k; };
};
