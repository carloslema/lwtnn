#include "lwtnn/Graph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/NNLayerConfig.hh"

#include <iostream>

#include <unistd.h>

lwt::GraphConfig dummy_config() {
  using namespace lwt;
  GraphConfig config;
  std::vector<Input> dummy_inputs{ {"one", 0, 1}, {"two", 0, 1} };
  config.inputs = {{"one", dummy_inputs}, {"two", dummy_inputs}};
  typedef NodeConfig::Type Type;
  config.nodes.push_back({Type::INPUT, {0}, 2});
  config.nodes.push_back({Type::INPUT, {1}, 2});
  config.nodes.push_back({Type::CONCATENATE, {0, 1}});
  config.nodes.push_back({Type::FEED_FORWARD, {2}, 0});
  config.nodes.push_back({Type::FEED_FORWARD, {3}, 0});

  LayerConfig dense {
    {0, 0, 0, 1,  0, 0, 1, 0,  0, 1, 0, 0,  1, 0, 0, 0}};
  dense.activation = Activation::LINEAR;
  dense.architecture = Architecture::DENSE;
  config.layers.push_back(dense);
  return config;
}

int main(int argc, char* argv[]) {
  lwt::GraphConfig config;
  if (isatty(fileno(stdin))) {
    config = dummy_config();
  } else {
    config = lwt::parse_json_graph(std::cin);
  }
  int node_number = -1;
  if (argc > 1) node_number = atoi(argv[1]);
  using namespace lwt;
  std::vector<size_t> inputs_per_node;
  for (const auto& innode: config.inputs) {
    inputs_per_node.push_back(innode.variables.size());
  }
  DummySource source(inputs_per_node);

  lwt::Graph graph(config.nodes, config.layers);
  if (node_number < 0) {
    std::cout << graph.compute(source) << std::endl;
  } else {
    std::cout << graph.compute(source, node_number) << std::endl;
  }
  return 0;
}
