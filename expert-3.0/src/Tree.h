/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EXP_TREE_H
#define EXP_TREE_H

/*
 *----------------------------------------------------------------------------
 *
 * class Tree
 *
 * Template tree container class
 *
 *---------------------------------------------------------------------------- 
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>

template<class T> class Tree
{
 public:
  
  struct Tnode 
  {
    Tnode* parent;
    std::vector<Tnode*> childv;
    long key;
    T data;
  };

  typedef typename std::map<long, Tnode*>::const_iterator MCI;

  virtual ~Tree();
  
  void   add_node(long key, T data, long parent_key = -1);
  void   del_node(long key);

  void   get_rootv(std::vector<long>& out) const;
  void   get_childv(long key, std::vector<long>& out) const;
  long   get_parent(long key) const;

  T      get_data(long key) const; 
  long   get_size() const { return keym.size(); }
  void   get_keyv(std::vector<long>& out) const;
  // get keys in depth-first search order
  void   get_dfkeyv(std::vector<long>& out, long key = -1) const;

  bool   is_key(long key);

private:
  
  std::map<long, Tnode*> keym;
  std::vector<Tnode*>    rootv;
};


template<class T>
Tree<T>::~Tree()
{
  MCI it;  

  for ( it = keym.begin(); it != keym.end() ; it++ )
    delete it->second;
}

template<class T> void
Tree<T>::add_node(long key, T data, long parent_key)
{
  // check whether key is new key and whether parent_key already exists
  if ( key < 0 || 
       keym.find(key) !=  keym.end() ||
       (parent_key != -1 && keym.find(parent_key) ==  keym.end()) )
    {
      std::cerr << "EXPERT: Error in Tree<T>::add_node(long, T, long)." << std::endl;
      std::cerr << "keym.size()=" << keym.size() << " key=" << key << " parent_key=" << parent_key << std::endl;
      if (keym.find(key) != keym.end())
          std::cerr << "keym.find(key) already exists!" << std::endl;
      if (parent_key != -1 && keym.find(parent_key) ==  keym.end())
          std::cerr << "keym.find(parent_key) missing!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
 
  Tnode* node = new Tnode();
  
  if ( parent_key == -1 )
    {
      rootv.push_back(node);
      node->parent = NULL;
    }
  else
    {
      keym[parent_key]->childv.push_back(node);
      node->parent = keym[parent_key];
    }
  node->data = data;
  node->key  = key;
  keym[key] = node;
}

template<class T> void
Tree<T>::del_node(long key)
{
  if ( keym.find(key) !=  keym.end() ) 
    {
      std::cerr << "EXPERT: Error in Tree<T>::del_node(long)." << std::endl;
      std::exit(EXIT_FAILURE);
    }
  
  Tnode* node = keym[key];
  Tnode* parent = node->parent;

  // remove from parent's list of children
  if ( parent != NULL )
      parent->childv.erase(find(parent->childv.begin(), parent->childv.end(), node));
  else
      rootv.erase(find(rootv.begin(), rootv.end(), node));
    
  // delete the node's children
  for ( long i = 0; i < node->childv.size(); i++ )
    this->del_node(node->childv[i]->key);

  // delete node
  keym.erase(keym.find(key));
  delete node;
}


template<class T> void
Tree<T>::get_rootv(std::vector<long>& out) const
{
  out.clear();

  for (size_t i = 0; i < rootv.size(); i++ ) 
    out.push_back(rootv[i]->key);
}

template<class T> void
Tree<T>::get_childv(long key, std::vector<long>& out) const
{
  out.clear();

  MCI result = keym.find(key);

  if ( result ==  keym.end() ) 
    {
      std::cerr << "EXPERT: Error in Tree<T>::get_childv(long, std::vector<long>&)." << std::endl;
      std::exit(EXIT_FAILURE);
    }
  
  for (size_t i = 0; i < result->second->childv.size(); i++ ) 
    out.push_back(result->second->childv[i]->key);  
}

template<class T> long 
Tree<T>::get_parent(long key) const 
{
  MCI result = keym.find(key);
 
  if ( result ==  keym.end() ) 
    {
      std::cerr << "EXPERT: Error in Tree<T>::get_parent(long)." << std::endl;
      std::exit(EXIT_FAILURE);
    }
  else if ( result->second->parent == NULL )
    {
      return -1;
    }
  else 
    {
      return result->second->parent->key;
    }
  return -1;
}


template<class T> void
Tree<T>::get_keyv(std::vector<long>& out) const
{
  //MCI result = keym.begin();

  for (MCI it = keym.begin(); it != keym.end() ; it++ ) 
    out.push_back(it->second->key);
}

template<class T> void
Tree<T>::get_dfkeyv(std::vector<long>& out, long key) const
{
  if ( key == -1 )
    out.clear();

  std::vector<long> keyv;

  if ( key == -1 )
    this->get_rootv(keyv);
  else
    this->get_childv(key, keyv);
  
    
  for ( size_t i = 0; i < keyv.size(); i++ )
    {
      out.push_back(keyv[i]);
      this->get_dfkeyv(out, keyv[i]);
    }
}

template<class T> T
Tree<T>::get_data(long key) const
{
  MCI result = keym.find(key);
  
  if ( result == keym.end() ) 
    {
      std::cerr << "EXPERT: Error in Tree<T>::get_data(long)." << std::endl;
      std::exit(EXIT_FAILURE);
    }
  return result->second->data;
}

template<class T> bool
Tree<T>::is_key(long key)
{
  return keym.find(key) == keym.end();
}


#endif
