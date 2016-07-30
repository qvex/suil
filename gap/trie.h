//
// Created by dc on 6/6/16.
//

#ifndef GAR_TRIE_H
#define GAR_TRIE_H

#include <memory>
#include <vector>
#include <string>
#include <algorithm>

namespace gap {

    template<typename ContentType>
    class Trie {
    protected:
        typedef std::shared_ptr<ContentType> ContentPtr;
        typedef std::function<bool(ContentPtr, const std::string&, uint)> LazySearchCb;

        class Node {
        public:
            typedef std::shared_ptr<Node>   Ptr;
            typedef typename std::vector<Node::Ptr>::iterator ChildIt;

            Node(const char& entry)
                    : entry_(entry),
                      content_(nullptr)
            {}

            ContentPtr content() {
                return content_;
            }

            void setContent(ContentPtr content) {
                content_ = content;
            }

            Node::Ptr find(const char c) {
                for(auto node: children_) {
                    if (node->entry() == c) {
                        return node;
                    }
                }
                return nullptr;
            }

            Node::ChildIt findIt(const char c) {
                for(Node::ChildIt it = children_.begin(); it != children_.end(); it++) {
                    if ((*it)->entry_ == c) {
                        return it;
                    }
                }
                return children_.end();
            }

            void  appendChild(Node::Ptr child) {
                children_.push_back(child);
            }

            std::vector<Node::Ptr> allChildren() {
                return children_;
            }

            const char& entry() const {
                return entry_;
            }

            const bool empty() const {
                return children_.empty();
            }

            void remove(Node::Ptr node) {
                if (node->empty() && node->content() == nullptr) {
                    children_.erase(findIt(node->entry()));
                }
            }

            static void dump(Node::Ptr node, std::string tab) {
                if (!node->empty()) {
                    printf("%s%c=[", tab.c_str(), node->entry_);
                    for (Node::Ptr ite: node->children_) {
                        printf("(%c:%d) ", ite->entry_, ite->content() != nullptr);
                    }
                    printf("]\n");
                    for (Node::Ptr ite: node->children_) {
                        dump(ite, tab+" ");
                    }
                }
            }

        private:
            ContentPtr                  content_;
            std::vector<Node::Ptr>      children_;
            char                        entry_;
        };

    public:
        Trie()
          : root_(new Node('\0'))
        {
        }

        bool add(const std::string& key, ContentPtr content) {
            typename Node::Ptr ite = root_;
            if (key.length() != 0) {
                for (auto entry: key) {
                    typename Node::Ptr child = ite->find(entry);
                    if (child != nullptr) {
                        ite = child;
                    } else {
                        typename  Node::Ptr tmp(new Node(entry));
                        ite->appendChild(tmp);
                        ite = tmp;
                    }
                }
            }

            if (ite->content() == nullptr) {
                ite->setContent(content);
                return true;
            }
            return false;
        }

        ContentPtr search(const std::string& key) {
            typename Node::Ptr ite = root_;
            for(auto entry: key) {
                if (ite == nullptr) {
                    return nullptr;
                }
                ite = ite->find(entry);
            }

            return ite? ite->content(): nullptr;
        }

        ContentPtr lazySearch(const std::string& key, LazySearchCb lazy) {
            if (lazy == nullptr) {
                return search(key);
            }

            typename Node::Ptr ite = root_;
            for(int i = 0; i < key.length(); i++) {
                if (ite == nullptr) {
                    return nullptr;
                }
                ite = ite->find(key[i]);
                if (ite != nullptr && ite->content() != nullptr) {
                    if(lazy(ite->content(), key, i)) {
                        break;
                    }
                }
            }

            return ite == nullptr? nullptr : ite->content();
        }

        void erase(const std::string& key) {
            if (key.length() > 0) {
                doErase(key, 1, root_->find(key[0]));
            }
        }

        void dump() {
            Node::dump(root_, "");
        }

    private:
        void doErase(const std::string& key, std::size_t pos, typename Node::Ptr ite) {
            if (ite != nullptr && key.length() == pos) {
                ite->setContent(nullptr);
            } else if(ite != nullptr){
                typename Node::Ptr tmp = ite->find(key[pos]);
                if (tmp != nullptr) {
                    doErase(key, pos + 1, tmp);
                    ite->remove(tmp);
                }
            }
        }

        typename  Node::Ptr       root_;
    };
}
#endif //GAR_TRIE_H
