#pragma once

#ifdef _DEBUG
#include <iostream>
#endif // DEBUG

namespace ds
{
    template <class T>
    struct TwoThreeNode
    {
        T k1, k2;                               //Key values, 2-node has 1 key while 3-node has 2 keys
        TwoThreeNode<T>* left;
        TwoThreeNode<T>* middle;                //pointers to children
        TwoThreeNode<T>* right;
        int n;                                 //number of keys
    };

    template <class T>
    struct RuntimeInfo
    {
        T midValue{ NULL };
        TwoThreeNode<T>* child;

        RuntimeInfo()
        {
            child = NULL;
        }

        RuntimeInfo(TwoThreeNode<T>* c)
        {
            child = c;
        }

        RuntimeInfo(TwoThreeNode<T>* c, T m)
        {
            child = c;
            midValue = m;
        }

        template<class T>
        RuntimeInfo<T> operator = (RuntimeInfo<T>& rti)
        {
            if (&rti != this)
            {
                child = rti.child;
                midValue = rti.midValue;
            }

            return *this;
        }

        template<class T>
        RuntimeInfo(RuntimeInfo<T>& rti)
        {
            child = rti.child;
            midValue = rti.midValue;
        }
    };

    template <class T>
    class TwoThreeTree
    {
    public:
        TwoThreeNode<T>* root;

    public:
        TwoThreeTree()
        {
            root = NULL;
        }

        ~TwoThreeTree()
        {
            if (root != NULL)
            {
                destroy(root);
            }
        }

        void destroy(TwoThreeNode<T>* r)
        {
            if (r != NULL)
            {
                destroy(r->left);
                destroy(r->middle);
                destroy(r->right);
                delete r;
            }
        }

        bool insert(T d)
        {
            auto temp = searchFor(d);

            if (temp == nullptr) //d doesnt exist
            {
                TwoThreeNode<T>* p = root; //pointer to parent

                RuntimeInfo<T> s1 = insert(root, d, p);

                if (s1.child != NULL)
                {
                    TwoThreeNode<T>* temp = new TwoThreeNode<T>;

                    temp->k1 = s1.midValue;
                    temp->n = 1;

                    temp->left = root;
                    temp->middle = s1.child;

                    temp->right = NULL;
                    root = temp;
                }

                return true;
            }

            return false;
        }

        bool deleteNode(T d)
        {
            if (searchFor(d) == nullptr) return false;
            TwoThreeNode<T>* p = root;     //Parent pointer will be used for rotation and merging purposes

            _delete(root, d, p);
            return true;
        }

        TwoThreeNode<T>* searchFor(T item)
        {
            return search(root, item);
        }

#ifdef _DEBUG
        void print() {
            _print(this->root);
            std::cout << std::endl;
        }
#endif

    private:
        enum class ROTATEDIR
        {
            IMPOSSIBLE = 0,
            RIGHT,
            LEFT
        };

        ROTATEDIR isRotationPossible(TwoThreeNode<T>* p, TwoThreeNode<T>* r)
        {
            if (p != r)
            {
                if (p->n == 1)    //parent is a 2-node
                {
                    if (r->n == 0)
                    {
                        if ((p->left == r) && (p->middle->n == 2))
                            return ROTATEDIR::LEFT;

                        else if ((p->middle == r) && (p->left->n == 2))
                            return ROTATEDIR::RIGHT;
                    }
                    else
                    {
                        if (!((p->left->n == 2) && (p->middle->n == 2)))
                        {
                            if (p->left == r)
                                return ROTATEDIR::RIGHT;

                            else
                                return ROTATEDIR::LEFT;
                        }
                    }
                }
                else //parent is a 3-node
                {
                    if (r->n == 0)
                    {
                        if ((p->left == r) && (p->middle->n == 2))
                        {
                            return ROTATEDIR::LEFT;
                        }

                        else if (p->middle == r)
                        {
                            if (p->left->n == 2)
                                return ROTATEDIR::RIGHT;

                            else if (p->right->n == 2)
                                return ROTATEDIR::LEFT;
                        }

                        else
                        {
                            if (p->middle->n == 2)
                                return ROTATEDIR::RIGHT;
                        }
                    }
                    else                //If OVERFLOW had occured at r
                    {
                        if ((p->left == r) && (p->middle->n == 1))
                            return ROTATEDIR::RIGHT;


                        else if (p->middle == r)
                        {
                            if (p->left->n == 1)
                                return ROTATEDIR::LEFT;

                            else if (p->right->n == 1)
                                return ROTATEDIR::RIGHT;
                        }
                        else
                        {
                            if (p->middle->n == 1)
                                return ROTATEDIR::LEFT;
                        }
                    }
                }
            }

            return ROTATEDIR::IMPOSSIBLE;
        }

        RuntimeInfo<T> rotateRight(TwoThreeNode<T>* p, TwoThreeNode<T>* r, T d, TwoThreeNode<T>* child)
        {
            if (r->n == 0) //root is empty
            {
                if ((p->n == 2) && (p->right == r))
                {
                    r->k1 = p->k2;
                    r->n = 1;
                    p->k2 = p->middle->k2;
                    p->middle->n = 1;
                    p->right->left = p->middle->right;
                }
                else
                {
                    r->k1 = p->k1;
                    r->n = 1;
                    p->k1 = p->left->k2;
                    p->left->n = 1;
                    p->middle->left = p->left->right;
                }
            }
            else
            {
                if (p->n == 2 && p->middle == r)
                {
                    p->right->k2 = p->right->k1;
                    p->right->k1 = p->k2;

                    if (d < r->k1)
                    {
                        p->k2 = r->k2;
                        r->k2 = r->k1;
                        r->k1 = d;
                    }
                    else if (d < r->k2)
                    {
                        p->k2 = r->k2;
                        r->k2 = d;
                    }
                    else
                    {
                        p->k2 = d;
                    }

                    p->right->right = p->right->middle;
                    p->right->middle = p->right->left;
                    p->right->left = child;
                    p->right->n = 2;
                }

                else
                {
                    p->middle->k2 = p->middle->k1;
                    p->middle->k1 = p->k1;
                    if (d < r->k1)
                    {
                        p->k1 = r->k2;
                        r->k2 = r->k1;
                        r->k1 = d;
                    }
                    else if (d < r->k2)
                    {
                        p->k1 = r->k2;
                        r->k2 = d;
                    }
                    else
                    {
                        p->k1 = d;
                    }
                    p->middle->right = p->middle->middle;
                    p->middle->middle = p->middle->left;
                    p->middle->left = child;
                    p->middle->n = 2;
                }
            }

            return (NULL);
        }

        RuntimeInfo<T> rotateLeft(TwoThreeNode<T>* p, TwoThreeNode<T>* r, T d, TwoThreeNode<T>* child)
        {
            if (r->n == 0) //root is empty
            {
                if ((p->n == 2) && (p->middle == r))
                {
                    r->k1 = p->k2;
                    r->n = 1;
                    p->k2 = p->right->k1;
                    p->right->k1 = p->right->k2;
                    p->right->n = 1;
                    p->middle->middle = p->right->left;
                    p->right->left = p->right->middle;
                    p->right->middle = p->right->right;
                }

                else
                {
                    r->k1 = p->k1;
                    r->n = 1;
                    p->k1 = p->middle->k1;
                    p->middle->k1 = p->middle->k2;
                    p->middle->n = 1;
                    p->left->middle = p->middle->left;
                    p->middle->left = p->middle->middle;
                    p->middle->middle = p->middle->right;
                }
            }
            else
            {
                if (p->n == 2 && p->right == r) {
                    p->middle->k2 = p->k2;

                    if (d < r->k1)
                    {
                        p->k2 = d;
                    }
                    else if (d < r->k2)
                    {
                        p->k2 = r->k1;
                        r->k1 = d;
                    }
                    else
                    {
                        p->k2 = r->k1;
                        r->k1 = r->k2;
                        r->k2 = d;
                    }

                    p->middle->right = p->right->left;
                    p->right->left = p->right->middle;
                    p->right->middle = p->right->right;
                    p->right->right = child;
                    p->middle->n = 2;
                }

                else
                {
                    p->left->k2 = p->k1;

                    if (d < r->k1)
                    {
                        p->k1 = d;
                    }
                    else if (d < r->k2)
                    {
                        p->k1 = r->k1;
                        r->k1 = d;
                    }
                    else
                    {
                        p->k1 = r->k1;
                        r->k1 = r->k2;
                        r->k2 = d;
                    }

                    p->left->right = p->middle->left;
                    p->middle->left = p->middle->middle;
                    p->middle->middle = p->middle->right;
                    p->middle->right = child;
                    p->left->n = 2;
                }
            }

            return (NULL);
        }

        RuntimeInfo<T> insert(TwoThreeNode<T>*& r, T d, TwoThreeNode<T>* p)
        {
            if (r == nullptr)               //root is empty, insert as root, root becomes a 2-node
            {
                TwoThreeNode<T>* temp = new TwoThreeNode<T>;

                temp->k1 = d;
                temp->left = temp->middle = temp->right = NULL;

                temp->n = 1;
                r = temp;

                return (NULL);
            }
            else if (r->left == nullptr)  //root is a leaf node
            {
                if (r->n == 1)           //root is a 2-node, then make it a 3-node
                {
                    if (d < r->k1)
                    {
                        r->k2 = r->k1;
                        r->k1 = d;
                    }

                    else
                        r->k2 = d;

                    r->n = 2;

                    return (NULL);
                }
                else                        //root is a 3-node
                {
                    RuntimeInfo<T> s1;


                    ROTATEDIR rd = isRotationPossible(p, r);

                    if (rd == ROTATEDIR::RIGHT)
                        s1 = rotateRight(p, r, d, NULL);

                    else if (rd == ROTATEDIR::LEFT)
                        s1 = rotateLeft(p, r, d, NULL);

                    else
                        s1 = split3node(r, d, NULL); //cant rotate, try splitting

                    return s1;
                }
            }
            else //root is a non-leaf node
            {
                if (r->n == 1) //root is a 2-node
                {
                    if (d < r->k1)
                    {
                        RuntimeInfo<T> s1 = insert(r->left, d, r);

                        if (s1.child != nullptr)
                        {
                            r->k2 = r->k1;
                            r->k1 = s1.midValue;

                            r->right = r->middle;
                            r->middle = s1.child;

                            r->n = 2;
                        }
                    }
                    else if (d > r->k1)
                    {
                        RuntimeInfo<T> s1 = insert(r->middle, d, r);

                        if (s1.child != NULL)
                        {
                            r->k2 = s1.midValue;
                            r->right = s1.child;
                            r->n = 2;
                        }
                    }

                    return (NULL);
                }
                else                            //root is a 3-node
                {
                    RuntimeInfo<T> s1;

                    if (d < r->k1)
                        s1 = insert(r->left, d, r);
                    else if (d < r->k2)
                        s1 = insert(r->middle, d, r);
                    else
                        s1 = insert(r->right, d, r);

                    if (s1.child != NULL)
                    {
                        ROTATEDIR rd = isRotationPossible(p, r);

                        if (rd == ROTATEDIR::RIGHT)
                            s1 = rotateRight(p, r, s1.midValue, s1.child);

                        else if (rd == ROTATEDIR::LEFT)
                            s1 = rotateLeft(p, r, s1.midValue, s1.child);

                        else
                            s1 = split3node(r, s1.midValue, s1.child); //cant rotate, try splitting

                        return s1;
                    }

                    return (NULL);
                }
            }
        }

        RuntimeInfo<T> split3node(TwoThreeNode<T>* current, T k, TwoThreeNode<T>* child)
        {
            T mid;
            TwoThreeNode<T>* temp = new TwoThreeNode<T>;
            temp->n = 1;
            temp->left = temp->middle = temp->right = NULL;

            if (k < current->k1)
            {
                mid = current->k1;

                temp->k1 = current->k2;
                current->k1 = k;

                if (current->left != NULL)
                {
                    temp->left = current->middle;
                    temp->middle = current->right;
                    current->middle = child;
                }
            }
            else if (k < current->k2)
            {
                mid = k;

                temp->k1 = current->k2;

                if (current->left != NULL)
                {
                    temp->left = child;
                    temp->middle = current->right;
                }
            }
            else
            {
                mid = current->k2;

                temp->k1 = k;

                if (current->left != NULL)
                {
                    temp->left = current->right;
                    temp->middle = child;
                }
            }

            current->right = NULL;
            current->n = 1;

            RuntimeInfo<T> s1(temp, mid);
            return s1;
        }

        RuntimeInfo<T> merge(TwoThreeNode<T>* p, TwoThreeNode<T>*& r, TwoThreeNode<T>* child)
        {
            if ((p->n == 2) && (p->right == r))
            {
                p->middle->k2 = p->k2;
                p->middle->n = 2;
                p->middle->right = child;

                child = NULL;
            }
            else
            {
                if (p->left == r)
                {
                    p->middle->k2 = p->middle->k1;
                    p->middle->k1 = p->k1;
                    p->middle->n = 2;
                    p->middle->right = p->middle->middle;
                    p->middle->middle = p->middle->left;
                    p->middle->left = child;
                    child = p->middle;
                    p->left = p->middle;
                    p->middle = p->right;
                }
                else
                {
                    p->left->k2 = p->k1;
                    p->left->n = 2;
                    p->left->right = child;
                    child = p->left;
                    p->middle = p->right;
                }

                if (p->n == 2)
                {
                    p->k1 = p->k2;
                    child = NULL;
                }
            }

            p->right = NULL;
            p->n--;

            r->left = r->middle = r->right = NULL;
            delete r;
            r = NULL;

            return (child);
        }

        RuntimeInfo<T> _delete(TwoThreeNode<T>* r, T d, TwoThreeNode<T>* p)
        {
            if (r != NULL) //empty check
            {
                if (r->left != NULL)
                {
                    RuntimeInfo<T> s1(NULL);

                    if (r->n == 1) //2-node
                    {
                        if (d != r->k1)
                        {
                            if (d < r->k1)
                            {
                                s1 = _delete(r->left, d, r);
                            }

                            else
                                s1 = _delete(r->middle, d, r);

                            if (s1.child == NULL)
                                return s1;
                        }
                    }
                    else //3-node
                    {
                        if ((d != r->k1) && (d != r->k2))
                        {
                            if (d < r->k1)
                            {
                                s1 = _delete(r->left, d, r);
                            }

                            else if (d < r->k2)
                            {
                                s1 = _delete(r->middle, d, r);
                            }

                            else
                                s1 = _delete(r->right, d, r);

                            if (s1.child == NULL)
                                return s1;
                        }
                    }

                    if (root->n == 0)
                    {
                        root->left = root->middle = root->right = NULL;
                        delete root;
                        root = s1.child;

                        r = root;

                        return (NULL);
                    }


                    if (s1.child != NULL)
                    {
                        ROTATEDIR rd = isRotationPossible(p, r);

                        if (rd == ROTATEDIR::RIGHT)
                            s1 = rotateRight(p, r, s1.midValue, s1.child);

                        else if (rd == ROTATEDIR::LEFT)
                            s1 = rotateLeft(p, r, s1.midValue, s1.child);

                        else
                            s1 = merge(p, r, s1.child); //cant rotate, try merging

                        return s1;
                    }

                    TwoThreeNode<T>* current;
                    T temp;

                    if ((r->n == 2) && (r->k2 == d))
                    {
                        temp = r->k2;
                        current = r->middle;

                        p = r;

                        while (current->left != NULL)
                        {
                            p = current;

                            if (current->n == 1)
                            {
                                current = current->middle;
                            }

                            else
                            {
                                current = current->right;
                            }
                        }


                        if (current->n == 1) //2-node
                        {
                            r->k2 = current->k1;
                            current->k1 = temp;
                        }

                        else //3-node
                        {
                            r->k2 = current->k2;
                            current->k2 = temp;
                        }
                    }


                    else
                    {
                        temp = r->k1;
                        current = r->left;

                        p = r;

                        while (current->left != NULL)
                        {
                            p = current;

                            if (current->n == 1)
                            {
                                current = current->middle;
                            }
                            else
                            {
                                current = current->right;
                            }
                        }


                        if (current->n == 1)
                        {
                            r->k1 = current->k1;
                            current->k1 = temp;
                        }

                        else if (current->n == 2)
                        {
                            r->k1 = current->k2;
                            current->k2 = temp;
                        }
                    }

                    current->n--;
                    r = current;
                }
                else if ((r->left == NULL) && ((r->k1 == d) || (r->k2 == d)))
                {
                    if (r->n == 2)
                    {
                        if (r->k1 == d)
                        {
                            r->k1 = r->k2;
                        }
                    }

                    r->n--;
                }

                if ((r->n == 0) && (p == r))
                {
                    delete r;
                    root = NULL;
                }
                else if (r->n == 0)
                {

                    RuntimeInfo<T> s1;

                    ROTATEDIR rd = isRotationPossible(p, r);


                    if (rd == ROTATEDIR::RIGHT)
                        s1 = rotateRight(p, r, s1.midValue, NULL);

                    else if (rd == ROTATEDIR::LEFT)
                        s1 = rotateLeft(p, r, s1.midValue, NULL);

                    else
                        s1 = merge(p, r, NULL);

                    return s1;
                }
            }

            return (NULL);
        }

        TwoThreeNode<T>* search(TwoThreeNode<T>* r, T d)
        {
            if (r != NULL)
            {
                if (r->n == 1)            //root is 2-node
                {
                    if (d == r->k1)
                        return r;

                    else if (d < r->k1)
                    {
                        return search(r->left, d);
                    }

                    else
                    {
                        return search(r->middle, d);
                    }
                }
                else                        //root is 3-node
                {
                    if (d == r->k1 || d == r->k2)
                        return r;

                    else if (d < r->k1)
                    {
                        return search(r->left, d);
                    }

                    else if (d < r->k2)
                    {
                        return search(r->middle, d);
                    }

                    else
                        return search(r->right, d);
                }
            }

            else
                return nullptr;       //Not found
        }

#ifdef _DEBUG
        void _print(ds::TwoThreeNode<T>* root)
        {
            if (root == nullptr) return;
            if (root->n == 1) //2 node
            {
                _print(root->left);
                std::cout << root->k1 << ' ';
                _print(root->middle);
            }
            else if (root->n == 2)
            {
                _print(root->left);
                std::cout << root->k1 << ' ';
                _print(root->middle);
                std::cout << root->k2 << ' ';
                _print(root->right);
            }
        }
#endif
    };
}
