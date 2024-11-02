#include "../include/privacy_solver.hpp"
#include "../include/pibt.hpp"


const std::string PP_MAPFSolver::SOLVER_NAME = "PP_MAPFSolver";

PP_MAPFSolver::PP_MAPFSolver(MAPF_Instance *_P) : MAPF_Solver(_P){
    solver_name = PP_MAPFSolver::SOLVER_NAME;
}

Node * PP_MAPFSolver::generate_random_node(){
    auto _V = G->getV();
    std::uniform_int_distribution<> distr(0, _V.size()); // define the range
    int index = distr(*MT);
    return _V[index];
}

PP_MAPFSolver::Agent * PP_MAPFSolver::generate_agent(int id){
    Node *real_start = P->getStart(id);
    Node *real_goal = P->getGoal(id);
    std::uniform_int_distribution<> distr(0, k - 1); // define the range
    int real_sub_agent_id = distr(*MT);
    SubAgent *current_sub_agent = NULL;
    Node *current_start = NULL;
    Node *current_goal = NULL;
    SubAgents *sub_agents = new SubAgents();
    Agent *agent = new Agent{
        id,
        real_sub_agent_id,
        sub_agents
    };

    for(int i = 0; i < k; ++i){
        if(i == real_sub_agent_id){
            current_start = real_start;
            current_goal = real_goal;
        }
        else{
            current_start = generate_random_node();
            current_goal = generate_random_node();
        }
        current_sub_agent = new SubAgent{
            i,
            current_start,
            current_goal
        };
        sub_agents->push_back(current_sub_agent);
        current_sub_agent = NULL;
        current_start = NULL;
        current_goal = NULL;
    }
    return agent;
}

void PP_MAPFSolver::generate_configs(Agents& agents, Config& start_config, Config& goal_config){
    int i = 0;
    int j = 0;
    Agent *agent = NULL;
    SubAgent *sub_agent = NULL;

    for(i = 0; i < P->getNum(); i++){
        agent = agents[i];
        for(j = 0; j < k; j++){
            sub_agent = (*agent->sub_agents)[j];
            start_config.push_back(sub_agent->start);
            goal_config.push_back(sub_agent->goal);
        }
    }
}

void PP_MAPFSolver::set_solution(Agents& agents, Plan& mock_solution){
    int i = 0;
    int j = 0;
    Node * current_node = NULL;
    Agent * current_agent = NULL;

    for (i = 0; i < mock_solution.size(); i++){
        Config current_mock_config;
        Config current_config;
        current_mock_config = mock_solution.get(i);
        for (j = 0; j < P->getNum(); j++){
            current_agent = agents[j];
            current_node = current_mock_config[j + current_agent->real_sub_agent_id];
            current_config.push_back(current_node);
        }
        solution.add(current_config);
    }
}

void PP_MAPFSolver::run()
{
    Agents agents;
    Config start_config;
    Config goal_config;
    Plan mock_solution;

    // initialize
    for (int i = 0; i < P->getNum(); ++i) {
        Agent * a = generate_agent(i);
        agents.push_back(a);
    }
    // convert to a new Problem, with n * k agents (all of the rest is the same).
    generate_configs(agents, start_config, goal_config);

    auto mock_problem = MAPF_Instance(P, start_config, goal_config, k * P->getNum());
    auto solver = std::make_unique<PIBT>(&mock_problem);
    solver->solve();

    mock_solution = solver->getSolution();
    set_solution(agents, mock_solution);

    if (solver->succeed()) {  // PIBT success
        solved = true;
    }
    else{
        solved = false;
    }
}


void PP_MAPFSolver::setParams(int argc, char* argv[])
{
    bool k_provided = false;
    struct option longopts[] = {
        {"mock-agents-num", required_argument, nullptr, 'k'},
        {nullptr, 0, nullptr, 0},
    };
    optind = 1;  // reset
    int opt, longindex;
    while ((opt = getopt_long(argc, argv, "k:", longopts, &longindex)) != -1) {
        switch (opt) {
        case 'k':
            k = std::stoi(optarg);
            k_provided = true;
            break;
        default:
            break;
        }
    }
    if (!k_provided) {
        std::cerr << "Error: The -k (or --mock-agents-num) option is required." << std::endl;
        exit(1);
    }
}

void PP_MAPFSolver::printHelp()
{
  std::cout << PP_MAPFSolver::SOLVER_NAME << "\n"
            << "  -k --mock-agents-num"
            << "              "
            << "number of mock agents to use\n"
            << std::endl;
}
