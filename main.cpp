#include <cstdlib>
#include <random>
#include <iomanip>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <chrono>

const int BLOCK_COUNT = 100;
const int BLOCK_MIN = 40;
const int BLOCK_MAX = 300;

const int ITEM_COUNT = 30;
const int ITEM_MIN = 5;
const int ITEM_MAX = 100;

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

  Problem() :
    blocks(BLOCK_COUNT),
    items(ITEM_COUNT)
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

class Solution {
  std::vector<int> order;
  std::weak_ptr<Problem> problem;
  float _fitnes = 0;
  float _age = 0;

public:
  using v_int_it = std::vector<int>::iterator;
  // Random solution
  Solution(std::shared_ptr<Problem> _problem)
    : order(_problem->items.size()), problem(_problem)
  {
    for (int i = 0; i < static_cast<int>(order.size()); i++) {
      order[i] = i;
    }
    std::random_device rd;
    std::mt19937 mt(rd());
    std::shuffle(order.begin(), order.end(), mt);
  }

  Solution(std::shared_ptr<Problem> _problem, Solution &left, Solution &right)
    : order(_problem->items), problem(_problem)
  {
    std::fill(order.begin(), order.end(), 0);
    int len = left.order.size();
    std::vector<int> usage(len, 0);
    int middle = len / 2;

    auto l_it = left.order.begin();
    auto l_it_middle = l_it + middle;
    auto r_it_begin = right.order.begin();
    auto r_it = r_it_begin + middle;

    auto next = [&](v_int_it &it) mutable -> int
      {
        while (usage[*it]) {
          it++;
        }

        int r = *it;
        it++;
        return r;
      };

    auto insert = [&](v_int_it &from, v_int_it &fallback, int pos) mutable {
      int index = usage[*from] ? next(fallback) : *from;
      from++;
      usage[index] = 1;
      order[pos] = index;
    };

    for (int i = 0; i < middle; i++) {
      insert(l_it, l_it_middle, i);
      insert(r_it, r_it_begin, middle + i);
    }

    if (len - middle * 2) {
      std::cout << "last:" << std::endl;
      insert(r_it, r_it_begin, len - 1);
    }

    mutation();
    fitness();
  }

  void add_age() {
    _age += 1;
  }

  int get_age() {
    return _age; 
  }

  void print_a(std::vector<int> &list) {
    std::cout << "[";
    int len = list.size();

    for (int i = 0; i < len - 1; i++) {
      std::cout << std::setw(2) << list[i] << ", ";
    }

    std::cout << std::setw(2) << list[len - 1] << "]" << std::endl;
  }

  void print() {
    print_a(order);
  }

  void mutation() {
    int index = std::rand() % (order.size() - 1);
    std::iter_swap(order.begin() + index, order.end() - 1);
  }

  float fitness() {
    if (_fitnes != 0) {
      return _fitnes;
    }

    int total_used  = 0;
    int total_used_blocks = 0;
    auto it = order.cbegin();
    auto end = order.cend();

    if (auto pr = problem.lock()) {
      for (auto block : pr->blocks) {
        int usage = 0;
        int cur_val = pr->items[*it];
	total_used_blocks += block;

        while (usage + cur_val <= block) {
          usage += cur_val;
          total_used += cur_val;
          it++;
          if (it == end) {
            return static_cast<float> (total_used) /
	      static_cast<float> (total_used_blocks);
          }
          cur_val = pr->items[*it];
        }
      }

      _fitnes = static_cast<float>(total_used) / static_cast<float>(total_used_blocks);
      return _fitnes;
    }

    return -1;
  }
};

const int POPULATION_SIZE = 80;

class Population
{
  std::array<std::shared_ptr<Solution>, POPULATION_SIZE> population;
  std::weak_ptr<Problem> problem;
  int epoch_number = 0;

public:

  Population(std::shared_ptr<Problem> _problem) : problem(_problem) {
    if (auto sp = problem.lock()) {
      for (int i = 0; i < POPULATION_SIZE; i++) {
        population[i] = std::make_shared<Solution>(sp);
      }
    }

    sort();
  }

  void sort() {
    auto cmp = [](std::shared_ptr<Solution> left,
                  std::shared_ptr<Solution> right) {
      return left->fitness() < right->fitness();
    };

    std::sort(population.begin(), population.end(), cmp);
  }

  void print() {
    for (auto &sol : population) {
      std::cout << "sol: " <<  sol->fitness() << " age: " << sol->get_age() << std::endl;
    }
  }

  void epoch() {
    std::cout << "[Epoch]: " << ++epoch_number << std::endl;
    int size = population.size();
    int last = size - 1;
    int count = size / 4;
    auto pr = problem.lock();

    for (int i = 0; i < count; i++) {
      population[i * 2] = std::make_shared<Solution>(pr, *population[last - i * 2], *population[last - i * 2 - 1]);
      population[i * 2 + 1] = std::make_shared<Solution>(pr, *population[last - i * 2 - 1], *population[last - i * 2]);
    }

    for (auto &p : population) {
      p->add_age();
    }

    sort();
    print();
    std::cout << std::endl;
  }
};

int main() {
  std::srand(std::time(0));
  auto pr = std::make_shared<Problem>();
  Population p(pr);
  for (int i = 0; i < 10; i++) {
    p.epoch();
  }
  return EXIT_SUCCESS;
};
