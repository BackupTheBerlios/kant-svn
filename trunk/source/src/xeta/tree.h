/***************************************************************************
 *   Copyright (C) 2004 by Daniel Albuschat <dalbuschat@zynot.org>         *
 *                                                                         *
 * This file is part of the Xeta package manager.                          *
 *                                                                         *
 * Xeta is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU Lesser General Public License as          *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * Xeta is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this software; if not, write to the Free Software            *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/

#ifndef XETA_TREE_H_166625497
#define XETA_TREE_H_166625497
#include <vector>
#include <ostream>
#include <iostream>

template<typename It>
void print( It begin, It end, std::string indent = "" ) {
  for( It i = begin; i != end; ++i )
    std::cout << indent << i->get_data() << '\n';
}

template<typename It>
void print_tree( It begin, It end, std::string indent = "", std::ostream &stream = std::cout ) {
  for( It i = begin; i != end; ++i ) {
    stream << indent << *i << '\n';
    print_tree(i.first_child(),i.end_child(), indent+"\t");
  }
}

namespace xeta {

  template<typename T>
    class Tree {
      public:
        struct iterator;
        class Leaf;
        typedef std::vector<Leaf> Branch;
        class Leaf {
          private:
            typename Tree::iterator parent;
            Branch children;
            T data;
            friend class Tree<T>;
            friend class Tree<T>::iterator;
            friend class Tree<T>::const_iterator;
          public:
            Leaf( const T &data, Tree::iterator parent ): parent(parent), data(data) { }
            Leaf(): data(T()) { }
            const T &get_data() const
            { return data; }
        };
        struct const_iterator {
          public:
            const_iterator &operator++() { ++node; return *this; }
            const_iterator &operator++(int) { ++node; return *this; }
            const_iterator &operator--() { --node; return *this; }
            const_iterator &operator--(int) { --node; return *this; }

            bool compare( const const_iterator &rhs ) const
            { return branch == rhs.branch && (node == rhs.node || eof == rhs.eof ); }
            bool operator==( const const_iterator &rhs ) const
            { return  compare( rhs ); }
            bool operator!=( const const_iterator &rhs ) const
            { return !compare( rhs ); }

            const T &operator*() { return node->data; }
            const T *operator->() { return &node->data; }
            const_iterator(): branch(0), eof(true) { }
            const_iterator( const const_iterator &rhs ): branch(rhs.branch), node(rhs.node), eof(rhs.eof) { }
            friend class Tree<T>;
            const_iterator first_child() const
            { return const_iterator(node->children, node->children.begin()); }
            const_iterator end_child() const
            { return const_iterator(node->children, node->children.end()); }
          private:
            const_iterator( const Branch &b, typename Branch::const_iterator it ): branch(&b), node(it), eof(it==b.end()) { }
            const_iterator( const Branch &b, bool eof ): branch(&b), eof(eof) { }
            const Branch *branch;
            typename Branch::const_iterator node;
            bool eof;
        };
        struct iterator {
          public:
            iterator &operator++() { ++node; return *this; }
            iterator &operator++(int) { ++node; return *this; }
            iterator &operator--() { --node; return *this; }
            iterator &operator--(int) { --node; return *this; }

            bool compare( const iterator &rhs ) const
            { return branch == rhs.branch && (node == rhs.node || eof == rhs.eof ); }
            bool operator==( const iterator &rhs ) const
            { return  compare( rhs ); }
            bool operator!=( const iterator &rhs ) const
            { return !compare( rhs ); }

            T &operator*() { return node->data; }
            T *operator->() { return &node->data; }
            iterator(): branch(0), eof(true) { }
            iterator( const iterator &rhs ): branch(rhs.branch), node(rhs.node), eof(rhs.eof) { }
            friend class Tree<T>;
            iterator first_child()
            { return iterator(node->children, node->children.begin()); }
            iterator end_child()
            { return iterator(node->children, node->children.end()); }
          private:
            iterator( Branch &b, typename Branch::iterator it ): branch(&b), node(it), eof(it==b.end()) { }
            iterator( Branch &b, bool eof ): branch(&b), eof(eof) { }
            Branch *branch;
            typename Branch::iterator node;
            bool eof;
        };
        Tree() { }
        const_iterator begin() const
        { return const_iterator( root, root.begin() ); }
        const_iterator end() const
        { return const_iterator( root, root.end() ); }
        iterator begin()
        { return iterator( root, root.begin() ); }
        iterator end()
        { return iterator( root, root.end() ); }
        iterator insert( iterator it, const T &val ) 
        { return iterator(*it.branch,it.branch->insert(it.node,Leaf(val,it))); }
        template<typename It>
          iterator insert( iterator it, It b, It e ) { 
            iterator ins(it);
            for( It i = b; i != e; ++i )
              ins = insert(ins,*i);
            return ins;
          }
        iterator push_front( iterator it, const T &val ) {
          it.branch->push_front(Leaf(val,it)); 
          return iterator(*it.branch,it.branch->begin());
        }
        iterator push_back( iterator it, const T &val ) { 
          it.branch->push_back(Leaf(val,it)); 
          return iterator(*it.branch,--(it.branch->end()));
        }
        iterator push_back( const T &val ) 
        { return push_back(begin(), val); }
        iterator find( const T &t ) 
        { return find( t, begin(), end() ); }
        iterator find( const T &t, iterator b, iterator e ) {
          for( iterator i = b; i != e; i++ ) {
            if( *i == t )
              return i;
            iterator result = find(t,i.first_child(),i.end_child());
            if( result != end() )
              return result;
          }
          return end();
        }
        void clear()
        { root.clear(); }
        void print() const
        { ::print_tree( begin(), end() ); }
        iterator erase( iterator it ) { 
          iterator result(
              *it.branch,
              it.branch->erase(it.node)
            ); 
          return result;
        }
      private:
        Branch root;
    };

}

#endif

