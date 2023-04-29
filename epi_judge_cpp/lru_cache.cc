#include <list>
#include <map>
#include <unordered_map>
#include <vector>

#include "test_framework/generic_test.h"
#include "test_framework/serialization_traits.h"
#include "test_framework/test_failure.h"

namespace {
  class UsingMultipleHashes {
  public:
    UsingMultipleHashes(size_t capacity): m_capacity { capacity } {}

    int Lookup(int isbn) {
      const auto it = isbn_to_price.find(isbn);

      if (it == isbn_to_price.end())
        return -1;

      sequence_to_isbn.erase(isbn_to_sequence[isbn]);
      isbn_to_sequence[isbn] = ++m_sequence;
      sequence_to_isbn[m_sequence] = isbn;

      return it->second;
    }

    void Insert(int isbn, int price) {
      const auto is_full = isbn_to_price.size() >= m_capacity;
      const auto is_new = isbn_to_price.find(isbn) == isbn_to_price.end();

      if (is_full && is_new) {
        const auto expired_isbn = sequence_to_isbn.begin()->second;
        isbn_to_price.erase(expired_isbn);
        isbn_to_sequence.erase(expired_isbn);
        sequence_to_isbn.erase(sequence_to_isbn.begin());
      }

      if (is_new)
        isbn_to_price[isbn] = price;
      else
        sequence_to_isbn.erase(isbn_to_sequence[isbn]);

      isbn_to_sequence[isbn] = ++m_sequence;
      sequence_to_isbn[m_sequence] = isbn;
    }

    bool Erase(int isbn) {
      const auto it = isbn_to_price.find(isbn);

      if (it == isbn_to_price.end())
        return false;

      sequence_to_isbn.erase(isbn_to_sequence[isbn]);
      isbn_to_sequence.erase(isbn);
      isbn_to_price.erase(it);
      return true;
    }

  private:
    size_t m_capacity;
    int m_sequence = 0;
    std::unordered_map<int, int> isbn_to_price;
    std::unordered_map<int, int> isbn_to_sequence;
    std::map<int, int> sequence_to_isbn;
  };

  class UsingLinkedList {
    using RecentUse = int;
    struct PriceWithRecentUse {
      int price;
      std::list<RecentUse>::iterator recent_use;
    };

  public:
    UsingLinkedList(size_t capacity): m_capacity { capacity } {}

    int Lookup(int isbn) {
      const auto it = m_isbn_to_price.find(isbn);
      if (it == m_isbn_to_price.end()) return -1;

      update_recent_use(it->second.recent_use);
      return it->second.price;
    }

    void Insert(int isbn, int price) {
      const auto it = m_isbn_to_price.find(isbn);

      if (it != m_isbn_to_price.end()) {
        update_recent_use(it->second.recent_use);
        return;
      }

      if (m_isbn_to_price.size() == m_capacity) {
        m_isbn_to_price.erase(m_recent_uses.back());
        m_recent_uses.pop_back();
      }

      m_recent_uses.push_front(isbn);
      m_isbn_to_price[isbn] = { price, m_recent_uses.begin() };
    }

    bool Erase(int isbn) {
      const auto it = m_isbn_to_price.find(isbn);
      if (it == m_isbn_to_price.end()) return false;

      m_recent_uses.erase(it->second.recent_use);
      m_isbn_to_price.erase(it);
      return true;
    }

  private:
    void update_recent_use(std::list<RecentUse>::iterator it) {
      m_recent_uses.splice(m_recent_uses.begin(), m_recent_uses, it);
    }

  private:
    size_t m_capacity;
    std::list<RecentUse> m_recent_uses;
    std::unordered_map<int, PriceWithRecentUse> m_isbn_to_price;
  };

}

using LruCache = UsingLinkedList;

struct Op {
  std::string code;
  int arg1;
  int arg2;
};

namespace test_framework {
template <>
struct SerializationTrait<Op> : UserSerTrait<Op, std::string, int, int> {};
}  // namespace test_framework

void LruCacheTester(const std::vector<Op>& commands) {
  if (commands.empty() || commands[0].code != "LruCache") {
    throw std::runtime_error("Expected LruCache as first command");
  }
  LruCache cache(commands[0].arg1);

  for (int i = 1; i < commands.size(); i++) {
    auto& cmd = commands[i];
    if (cmd.code == "lookup") {
      int result = cache.Lookup(cmd.arg1);
      if (result != cmd.arg2) {
        throw TestFailure("Lookup: expected " + std::to_string(cmd.arg2) +
                          ", got " + std::to_string(result));
      }
    } else if (cmd.code == "insert") {
      cache.Insert(cmd.arg1, cmd.arg2);
    } else if (cmd.code == "erase") {
      bool result = cache.Erase(cmd.arg1);
      if (result != cmd.arg2) {
        throw TestFailure("Erase: expected " + std::to_string(cmd.arg2) +
                          ", got " + std::to_string(result));
      }
    } else {
      throw std::runtime_error("Unexpected command " + cmd.code);
    }
  }
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"commands"};
  return GenericTestMain(args, "lru_cache.cc", "lru_cache.tsv", &LruCacheTester,
                         DefaultComparator{}, param_names);
}
