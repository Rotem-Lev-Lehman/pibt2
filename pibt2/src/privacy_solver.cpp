#include "../include/privacy_solver.hpp"
#include "../include/pibt.hpp"


const std::string PP_MAPFSolver::SOLVER_NAME = "PP_MAPFSolver";

PP_MAPFSolver::PP_MAPFSolver(MAPF_Instance *_P) : MAPF_Solver(_P){
    solver_name = PP_MAPFSolver::SOLVER_NAME;
}

PP_MAPFSolver::Agent * PP_MAPFSolver::generate_agent(TasksDispatcher& tasks_dispatcher, size_t id){
    Config start_config;
    Config goal_config;
    // TODO - change the seed here as well since it randomizes the same for all agents.
    std::uniform_int_distribution<> distr(0, k - 1); // define the range
    size_t real_sub_agent_id = distr(*MT);
    SubAgent *current_sub_agent = NULL;
    SubAgents *sub_agents = new SubAgents();
    Agent *agent = new Agent{
        id,
        real_sub_agent_id,
        sub_agents
    };

    tasks_dispatcher.dispatch(k, start_config, goal_config);

    for(size_t i = 0; i < k; ++i){
        current_sub_agent = new SubAgent{
            i,
            start_config[i],
            goal_config[i]
        };
        sub_agents->push_back(current_sub_agent);
        current_sub_agent = NULL;
    }
    P->setStart(id, start_config[real_sub_agent_id]);
    P->setGoal(id, goal_config[real_sub_agent_id]);
    if(P->getStart(id) != start_config[real_sub_agent_id]){
        std::cout << "weirdly invalid set_start" << std::endl;
        exit(1);
    }
    if(P->getGoal(id) != goal_config[real_sub_agent_id]){
        std::cout << "weirdly invalid set_goal" << std::endl;
        exit(1);
    }
    return agent;
}

void PP_MAPFSolver::generate_configs(Agents& agents, Config& start_config, Config& goal_config){
    int i = 0;
    size_t j = 0;
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
            current_node = current_mock_config[j * k + current_agent->real_sub_agent_id];
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
    P->setFieldOfViewRadius(field_of_view_radius);
    TasksDispatcher tasks_dispatcher = TasksDispatcher(P);

    // initialize
    for (int i = 0; i < P->getNum(); ++i) {
        Agent * a = generate_agent(tasks_dispatcher, i);
        agents.push_back(a);
    }
    // convert to a new Problem, with n * k agents (all of the rest is the same).
    generate_configs(agents, start_config, goal_config);

    auto mock_problem = MAPF_Instance(P, start_config, goal_config, k * P->getNum());
    mock_problem.setFieldOfViewRadius(field_of_view_radius);
    auto solver = std::make_unique<PIBT>(&mock_problem);
    solver->solve();

    if (solver->succeed() && !solver->getSolution().validate(&mock_problem)) {
        std::cout << "error@privacy_solver: invalid results" << std::endl;
        exit(1);
    }

    // output result
    // TODO - change this to a variable.
    solver->makeLog("./intermidiate_results.txt");
    if (verbose) {
        std::cout << "save result as " << "./intermidiate_results.txt" << std::endl;
    }

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
    bool field_of_view_radius_provided = false;
    struct option longopts[] = {
        {"mock-agents-num", required_argument, nullptr, 'k'},
        {"field-of-view-radius", required_argument, nullptr, 'r'},
        {nullptr, 0, nullptr, 0},
    };
    optind = 1;  // reset
    int opt, longindex;
    while ((opt = getopt_long(argc, argv, ":r:k:", longopts, &longindex)) != -1) {
        switch (opt) {
        case 'k':
            k = std::stoi(optarg);
            k_provided = true;
            break;
        case 'r':
            field_of_view_radius = std::stoi(optarg);
            field_of_view_radius_provided = true;
            break;
        default:
            break;
        }
    }
    if (!k_provided) {
        std::cerr << "Error: The -k (or --mock-agents-num) option is required." << std::endl;
        exit(1);
    }
    if (!field_of_view_radius_provided) {
        std::cerr << "Error: The -r (or --field-of-view-radius) option is required." << std::endl;
        exit(1);
    }
}

void PP_MAPFSolver::printHelp()
{
  std::cout << PP_MAPFSolver::SOLVER_NAME << "\n"
            << "  -k --mock-agents-num"
            << "          "
            << "number of mock agents to use\n"
            << "  -r --field-of-view-radius"
            << "     "
            << "radius that other agents may see each other\n"
            << std::endl;
}

TasksDispatcher::TasksDispatcher(MAPF_Instance* _P): 
    P(_P), 
    available_starts(_P->getG()->getV()), 
    available_goals(_P->getG()->getV()),
    MT(_P->getMT())
{
    std::shuffle(available_starts.begin(), available_starts.end(), *MT);
    std::shuffle(available_goals.begin(), available_goals.end(), *MT);
}

/**
 * @brief A struct for defining an Unary Operator for checking if a node is in the field of view.
 * Used in the std::remove_if statement in the TasksDispatcher::dispatch function.
 */
struct IsInFieldOfView {
    IsInFieldOfView(Graph * g, Node * node, int field_of_view_radius) : 
        field_of_view{get_field_of_view(g, node, field_of_view_radius)} {}
    bool operator()(Node * other) {
        return std::find(field_of_view.begin(), field_of_view.end(), other) != field_of_view.end();
    }
    Nodes field_of_view;
};

void TasksDispatcher::dispatch(size_t k, Config &start_config, Config &goal_config){
    Node * current_start = NULL;
    Node * current_goal = NULL;

    for (size_t i = 0; i < k; i++){
        if(available_starts.size() == 0 || available_goals.size() == 0){
            std::cout << "error@TasksDispatcher: not enough nodes (0 nodes)." << std::endl;
            exit(1);
        }
        current_start = available_starts.back();
        current_goal = available_goals.back();
        // The available_starts and available_goals vectors are already in random
        // order, so just pop the last element into the configs.
        start_config.push_back(current_start);
        available_starts.pop_back();
        if (current_start->id == current_goal->id){
            // If they are the same node, use another node
            // (the one before last, so it will be easy to pop).
            if (available_goals.size() < 2){
                std::cout << "error@TasksDispatcher: not enough nodes (1 same node)." << std::endl;
                exit(1);
            }
            current_goal = available_goals[available_goals.size() - 2];
            available_goals.erase(available_goals.begin() + (available_goals.size() - 2));
        }
        else{
            available_goals.pop_back();
        }
        goal_config.push_back(current_goal);

        // Remove all of the field of view of current_goal and current_start from the available nodes:
        available_starts.erase(
            std::remove_if(available_starts.begin(), available_starts.end(), IsInFieldOfView(P->getG(), current_start, P->getFieldOfViewRadius())),
            available_starts.end()
        );
        available_goals.erase(
            std::remove_if(available_goals.begin(), available_goals.end(), IsInFieldOfView(P->getG(), current_goal, P->getFieldOfViewRadius())),
            available_goals.end()
        );
    }
}