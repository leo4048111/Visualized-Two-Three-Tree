#pragma once
#include "TwoThreeTree.hpp"
#include "imgui.h"

namespace TreeNodePositioning
{
    template<class T>
    class Node
    {
    public:
        T k1{NULL};
        T k2{NULL};
        int n{ NULL };

        float x{ 0 };
        int y{ 0 };
        float mod{ 0 };

        Node* parent{nullptr};
        Node* children[3]{ nullptr, nullptr, nullptr };

        float width{0.f};
        int height{0};

        bool isMarked{ false };
    public:
        Node() = default;
        ~Node() = default;

        int getChildrenCnt()
        {
            return !(this->children[0] == nullptr) + !(this->children[1] == nullptr) + !(this->children[2] == nullptr);
        }

        int getChildIdx(Node<T>* node)
        {
            if (node == nullptr) return -1;
            if (this->children[0] == node) return 0;
            if (this->children[1] == node) return 1;
            if (this->children[2] == node) return 2;

            return -1;
        }

        bool isLeaf()
        {
            return (this->getChildrenCnt() == 0);
        }

        bool isLeftMost()
        {
            if (this->parent == nullptr)
                return true;

            return this->parent->children[0] == this;
        }

        bool isRightMost()
        {
            if (this->parent == nullptr)
                return true;

            return this->parent->children[this->parent->getChildrenCnt() - 1] == this;
        }

        Node<T>* getPreviousSibling()
        {
            if (this->parent == nullptr || this->isLeftMost())
                return nullptr;

            return this->parent->children[this->parent->getChildIdx(this) - 1];
        }

        Node<T>* getNextSibling()
        {
            if (this->parent == nullptr || this->isRightMost())
                return nullptr;

            return this->parent->children[this->parent->getChildIdx(this) + 1];
        }

        Node<T>* getLeftMostSibling()
        {
            if (this->parent == nullptr)
                return nullptr;

            if (this->isLeftMost())
                return this;

            return this->parent->children[0];
        }

        Node<T>* getLeftMostChild()
        {
            if (this->getChildrenCnt() == 0)
                return nullptr;

            return this->children[0];
        }

        Node<T>* getRightMostChild()
        {
            if (this->getChildrenCnt() == 0)
                return nullptr;

            return this->children[this->getChildrenCnt() - 1];
        }
    };

    template<class T>
    class Helper
    {
    public:
        Helper() = delete;  //INCONSTRUCTIBLE
        Helper(Helper&&) = delete;
        Helper(const Helper&) = delete;
        Helper& operator=(const Helper&) = delete;

        static void calculate(ImVector<Node<T>*>& nodeList, const ds::TwoThreeNode<T>* twoThreeRoot, const float siblingSpacing, const float treeSpacing, const float size)
        {
            nodeList.clear();
            if (twoThreeRoot == nullptr) return;
            Node<T>* root = nullptr;
            //convert twoThreeNode to Node so that algorithm can run
            initialize(nodeList, twoThreeRoot, root, nullptr, -1, 0);
            //calculate initial x and mod value
            calculateInitialX(nodeList, root, siblingSpacing, treeSpacing, size);
            //adjust offscreen coords
            checkAllChildrenOnScreen(nodeList, root);
            //calculate final x coords
            calculateFinalX(root, 0.f);
        }

    private:
        static void initialize(ImVector<Node<T>*>& nodeList, const ds::TwoThreeNode<T>* twoThreeNode, Node<T>*& root, Node<T>* parent, int childIdx, int depth)
        {
            if (twoThreeNode == nullptr) return;

            Node<T>* node = new Node<T>();
            node->x = -1.f;
            node->y = depth;
            node->mod = 0.f;

            node->k1 = twoThreeNode->k1;
            node->k2 = twoThreeNode->k2;
            node->n = twoThreeNode->n;

            node->parent = parent;

            if (parent != nullptr) parent->children[childIdx] = node;

            nodeList.push_back(node);

            if (twoThreeNode->left != nullptr) initialize(nodeList, twoThreeNode->left, root, node, 0, depth + 1);
            if (twoThreeNode->middle != nullptr) initialize(nodeList, twoThreeNode->middle, root, node, 1, depth + 1);
            if (twoThreeNode->right != nullptr) initialize(nodeList, twoThreeNode->right, root, node, 2, depth + 1);

            if (depth == 0) root = node;
        }

        static void calculateInitialX(ImVector<Node<T>*>& nodeList, Node<T>* node, const float siblingSpacing, const float treeSpacing, const float size)
        {
            if (node == nullptr) return;
            calculateInitialX(nodeList, node->children[0], siblingSpacing, treeSpacing, size);
            calculateInitialX(nodeList, node->children[1], siblingSpacing, treeSpacing, size);
            calculateInitialX(nodeList, node->children[2], siblingSpacing, treeSpacing, size);

            if (node->isLeaf()) //no children
            {
                if (!node->isLeftMost()) //not left most
                    node->x = node->getPreviousSibling()->x + size + siblingSpacing;
                else
                    node->x = 0.f;
            }
            else if (node->getChildrenCnt() == 1) //has 1 child
            {
                if (node->isLeftMost())
                {
                    node->x = node->children[0]->x;
                }
                else
                {
                    node->x = node->getPreviousSibling()->x + size + siblingSpacing;
                    node->mod = node->x - node->children[0]->x;
                }
            }
            else
            {
                float mid = (node->getLeftMostChild()->x + node->getRightMostChild()->x) / 2;
                if (node->isLeftMost())
                {
                    node->x = mid;
                }
                else
                {
                    node->x = node->getPreviousSibling()->x + size + siblingSpacing;
                    node->mod = node->x - mid;
                }
            }

            if (node->getChildrenCnt() > 0 && !node->isLeftMost())
            {
                checkForConflicts(nodeList, node, treeSpacing, size);
            }
        }

        static void checkForConflicts(ImVector<Node<T>*>& nodeList, Node<T>* node, const float treeSpacing, const float size)
        {
            float minDistance = treeSpacing + size;
            float shiftValue = 0.f;

            float* nodeContour = (float*)malloc(sizeof(float) * (nodeList.size() + 1));
            for (int i = 0; i < nodeList.size(); i++) nodeContour[i] = FLT_MAX;
            int maxLeftContourKey = getLeftContour(node, 0.f, nodeContour);

            Node<T>* sibling = node->getLeftMostSibling();
            while (sibling != nullptr && sibling != node)
            {
                float* siblingContour = (float*)malloc(sizeof(float) * (nodeList.size() + 1));
                for (int i = 0; i < nodeList.size(); i++) siblingContour[i] = FLT_MIN;
                int maxRightContourKey = getRightContour(sibling, 0.f, siblingContour);

                for (int level = node->y + 1; level <= min(maxLeftContourKey, maxRightContourKey); level++)
                {
                    float distance = nodeContour[level] - siblingContour[level];
                    if (distance + shiftValue < minDistance) shiftValue = minDistance - distance;
                }

                if (shiftValue > 0)
                {
                    node->x += shiftValue;
                    node->mod += shiftValue;

                    centerNodesBetween(nodeList, sibling, node, treeSpacing, size);

                    shiftValue = 0;
                }

                sibling = sibling->getNextSibling();
                free(siblingContour);
            }

            free(nodeContour);
        }

        static int getLeftContour(Node<T>* node, float modSum, float* values)
        {
            if (node == nullptr) return -1;
            values[node->y] = fminf(values[node->y], node->x + modSum);
            modSum += node->mod;

            int depth = node->y;
            if (node->children[0] != nullptr) depth = max(depth, getLeftContour(node->children[0], modSum, values));
            if (node->children[1] != nullptr) depth = max(depth, getLeftContour(node->children[1], modSum, values));
            if (node->children[2] != nullptr) depth = max(depth, getLeftContour(node->children[2], modSum, values));
            return depth;
        }

        static int getRightContour(Node<T>* node, float modSum, float* values)
        {
            if (node == nullptr) return -1;
            values[node->y] = fmaxf(values[node->y], node->x + modSum);
            modSum += node->mod;

            int depth = node->y;
            if (node->children[0] != nullptr) depth = max(depth, getRightContour(node->children[0], modSum, values));
            if (node->children[1] != nullptr) depth = max(depth, getRightContour(node->children[1], modSum, values));
            if (node->children[2] != nullptr) depth = max(depth, getRightContour(node->children[2], modSum, values));
            return depth;
        }

        static void centerNodesBetween(ImVector<Node<T>*>& nodeList, Node<T>* leftNode, Node<T>* rightNode, const float treeSpacing, const float size)
        {
            int leftIdx = leftNode->parent->getChildIdx(leftNode);
            int rightIdx = leftNode->parent->getChildIdx(rightNode);

            int numNodesBetween = rightIdx - leftIdx - 1;

            if (numNodesBetween > 0)
            {
                float distanceBetweenNodes = (rightNode->x - rightNode->x) / (numNodesBetween + 1);
                int cnt = 1;
                for (int i = leftIdx + 1; i < rightIdx; i++)
                {
                    Node<T>* middleNode = leftNode->parent->children[i];
                    float desiredX = leftNode->x + (distanceBetweenNodes * cnt);
                    float offset = desiredX - middleNode->x;
                    middleNode->x += offset;
                    middleNode->mod += offset;
                    cnt++;
                }

                checkForConflicts(nodeList, leftNode, treeSpacing, size);
            }
        }

        static void checkAllChildrenOnScreen(ImVector<Node<T>*>& nodeList, Node<T>* node)
        {
            float* nodeContour = (float*)malloc(sizeof(float) * (nodeList.size() + 1));
            for (int i = 0; i < nodeList.size(); i++) nodeContour[i] = FLT_MAX;
            int maxLeftContourKey = getLeftContour(node, 0.f, nodeContour);

            float shiftAmount = 0.f;
            for (int i = node->y; i <= maxLeftContourKey; i++)
            {
                if (nodeContour[i] + shiftAmount < 0) shiftAmount = (nodeContour[i] * -1);
            }

            if (shiftAmount > 0)
            {
                node->x += shiftAmount;
                node->mod += shiftAmount;
            }

            free(nodeContour);
        }

        static void calculateFinalX(Node<T>* node, float modSum)
        {
            if (node == nullptr) return;
            node->x += modSum;
            modSum += node->mod;

            calculateFinalX(node->children[0], modSum);
            calculateFinalX(node->children[1], modSum);
            calculateFinalX(node->children[2], modSum);
            
            if (node->getChildrenCnt() == 0)
            {
                node->width = node->x;
                node->height = node->y;
            }
            else
            {
                float width = FLT_MIN;
                int height = INT_MIN;
                for (int i = 0; i < 3; i++)
                {
                    if(node->children[i] != nullptr)
                    {
                        width = max(width, node->children[i]->width);
                        height = max(height, node->children[i]->height);
                    }
                }

                node->width = width;
                node->height = height;
            }
        }
    };
}

