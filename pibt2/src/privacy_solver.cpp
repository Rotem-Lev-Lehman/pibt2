#include "../include/privacy_solver.hpp"


const std::string PP_MAPFSolver::SOLVER_NAME = "PP_MAPFSolver";

PP_MAPFSolver::PP_MAPFSolver(MAPF_Instance *_P) : MAPF_Solver(_P){
    solver_name = PP_MAPFSolver::SOLVER_NAME;
}



Agent * PP_MAPFSolver::generate_agent(int id){
    Node *real_start = P->getStart(id);
    Node *real_goal = P->getGoal(id);
    std::uniform_int_distribution<> distr(0, k - 1); // define the range
    int real_sub_agent_id = distr(MT);
    SubAgent *current_sub_agent = NULL;
    Node *current_start = NULL;
    Node *current_goal = NULL;
    SubAgents *sub_agents = new SubAgents();
    Agent *agent = new Agent(
        id,
        real_sub_agent_id,
        sub_agents
    );

    for(int i = 0; i < k; ++i){
        if(i == real_sub_agent_id){
            current_start = real_start;
            current_goal = real_goal;
        }
        else{
            current_start = generate_random_node();
            current_goal = generate_random_node();
        }
        current_sub_agent = new SubAgent(
            i,
            current_start,
            current_goal
        );
        sub_agents->push_back(current_sub_agent);
        current_sub_agent = NULL;
        current_start = NULL;
        current_goal = NULL;
    }
}

PP_MAPFSolver::run()
{
    Agents agents;

    // initialize
    for (int i = 0; i < P->getNum(); ++i) {
        Node* s = P->getStart(i);
        Node* g = P->getGoal(i);

        Agent* a = new Agent{i,                          // id

                            s,                          // current location
                            nullptr,                    // next location
                            g,                          // goal
                            0,                          // elapsed
                            d,                          // dist from s -> g
                            getRandomFloat(0, 1, MT)};  // tie-breaker
        A.push_back(a);
        occupied_now[s->id] = a;
    }
    solution.add(P->getConfigStart());
}
