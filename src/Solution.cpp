#include "Common.h"

#include <list>
#include <mutex>
#include <unordered_map>

using namespace std;

class LruCache : public ICache {
public:
  LruCache(
      shared_ptr<IBooksUnpacker> books_unpacker,
      const Settings& settings
  ) : books_unpacker(books_unpacker),
      settings(settings) {}

  BookPtr GetBook(const string& book_name) override {
    lock_guard guard(m);
    auto it = cached_books_by_name.find(book_name);
    if (it != cached_books_by_name.end()) {
      cached_books.splice(cached_books.end(), cached_books, it->second);
      return *(it->second);
    }
    
    auto new_book = books_unpacker->UnpackBook(book_name);
    size_t new_book_size = new_book->GetContent().size();
    if (new_book_size > settings.max_memory) {
      return move(new_book);
    }

    while (!cached_books.empty() && size + new_book_size > settings.max_memory) {
      size -= cached_books.front()->GetContent().size();
      cached_books_by_name.erase(cached_books.front()->GetName());
      cached_books.pop_front();
    }
    auto saved_book_iterator = cached_books.emplace(cached_books.end(), move(new_book));
    cached_books_by_name[(*saved_book_iterator)->GetName()] = saved_book_iterator;
    size += new_book_size;
    return *saved_book_iterator;
  }
private:
  mutex m;
  size_t size = 0;
  const shared_ptr<IBooksUnpacker> books_unpacker;
  const Settings settings;
  list<BookPtr> cached_books;
  unordered_map<string, list<BookPtr>::iterator> cached_books_by_name;
};


unique_ptr<ICache> MakeCache(
    shared_ptr<IBooksUnpacker> books_unpacker,
    const ICache::Settings& settings
) {
  return make_unique<LruCache>(books_unpacker, settings);
}
