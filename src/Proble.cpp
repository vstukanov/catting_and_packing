class Problem {
public:
  std::vector<int> blocks;
  std::vector<int> items;

  int total_space = 0;

  using int_a = std::vector<int>;

  void init_array(int_a &list, int min, int max) {
    int i = 0;
    int ll = list.size();

    for (i = 0; i < ll; i++) {
      list[i] = std::rand() % (max - min) + min;
    }
  }

  void print_array(int_a &list, const char * name) {
    std::cout << name << ":" << std::endl;
    for (auto item : list) {
      std::cout << item << " ";
    }
    std::cout << std::endl;
  }

  Problem(int block_count, int item_count) :
    blocks(block_count),
    items(item_count)
  {
    init_array(blocks, BLOCK_MIN, BLOCK_MAX);
    init_array(items, ITEM_MIN, ITEM_MAX);

    for (int i : blocks) {
      total_space += i;
    }

    // print_array(blocks, "Blocks");
    // print_array(items, "Items");
  }
};
