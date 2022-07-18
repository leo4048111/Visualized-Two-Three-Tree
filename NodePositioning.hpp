#pragma once
#include "TwoThreeTree.hpp"
#include "imgui.h"

namespace NodePositioning
{
    struct Node
    {
        int k1{ NULL };
        int k2{ NULL };
        int n{ NULL };

        Node* parent{ nullptr };
        Node* firstChild{ nullptr };
        Node* leftSibling{ nullptr };
        Node* rightSibling{ nullptr };
        Node* leftNeighbour{ nullptr };
        float x{ 0 };
        float y{ 0 };
        float prelim{ 0 };
        float modifier{ 0 };

        int column{ 0 };
        int row{ 0 };
    };

    inline Node* getLeftMost(Node* node, int row, const int depth)
    {
        if (depth <= row)
            return node;
        else if (node->firstChild == nullptr)
            return nullptr;
        else
        {
            Node* rightMost = node->firstChild;
            Node* leftMost = getLeftMost(rightMost, row + 1, depth);
            while ((leftMost == nullptr) && (rightMost->rightSibling != nullptr)) {
                rightMost = rightMost->rightSibling;
                leftMost = getLeftMost(rightMost, row + 1, depth);
            }
            return leftMost;
        }
    }

    inline void apportion(Node* node, const float spacing, const float size)
    {
        Node* leftMost = node->firstChild;
        Node* neighbor = node->leftNeighbour;

        int compareDepth = 1;
        float leftModSum, rightModSum;

        while ((leftMost != nullptr) && (neighbor != nullptr))
        {
            leftModSum = 0.0;
            rightModSum = 0.0;
            Node* ancestorLeftmost = leftMost;
            Node* ancestorNeighbour = neighbor;

            for (int i = 0; i < compareDepth; i++)
            {
                ancestorLeftmost = ancestorLeftmost->parent;
                ancestorNeighbour = ancestorNeighbour->parent;

                if ((ancestorLeftmost == nullptr) || (ancestorNeighbour == nullptr)) continue;

                rightModSum += ancestorLeftmost->modifier;
                leftModSum += ancestorNeighbour->modifier;
            }

            float moveDistance = (neighbor->prelim + leftModSum + spacing + size) - (leftMost->prelim + rightModSum);

            if (moveDistance > 0.0)
            {
                Node* tempPtr = node;
                int leftSibling = 0;

                while ((tempPtr != nullptr) && (tempPtr != ancestorNeighbour)) {
                    leftSibling = leftSibling + 1;
                    tempPtr = tempPtr->leftSibling;
                }

                if (tempPtr != nullptr)
                {
                    float portion = moveDistance / leftSibling;
                    tempPtr = node;
                    while (tempPtr == ancestorNeighbour) 
                    {
                        tempPtr->prelim = tempPtr->prelim + moveDistance;
                        tempPtr->modifier = tempPtr->modifier + moveDistance;
                        moveDistance = moveDistance - portion;
                        tempPtr = tempPtr->leftSibling;
                    }
                }
                else
                    return;
            }

            compareDepth += 1;
            if (leftMost->firstChild == nullptr)
                leftMost = getLeftMost(node, 0, compareDepth);//当前深度，查找第一个有子节点的，取最左子节点
            else
                leftMost = leftMost->firstChild;

            neighbor = leftMost->leftNeighbour;
        }
    }

    inline void firstWalk(Node* node, int row, const float spacing, const float size)
    {
        if (node->firstChild == nullptr) //leaf node
        {
            if (node->leftSibling != nullptr) //has left sibling
                node->prelim = node->leftSibling->prelim + spacing + size;
            else
                node->prelim = 0;
        }
        else //non-leaf node
        {
            Node* child = node->firstChild;
            float leftPrelim = 0;
            float rightPrelim = 0;
            firstWalk(child, row + 1, spacing, size);
            leftPrelim = child->prelim;
            while (child->rightSibling != nullptr) {
                child = child->rightSibling;
                firstWalk(child, row + 1, spacing, size);
            }
            rightPrelim = child->prelim;

            float midPoint = (leftPrelim + rightPrelim) / 2; //mid point should be right most + left most / 2

            if (node->leftSibling != nullptr)
            {
                node->prelim = node->leftSibling->prelim + spacing + size;
                node->modifier = node->prelim - midPoint;
                apportion(node, spacing, size);
            }
            else //no left sibling
                node->prelim =  midPoint;
        }
    }

    inline void secondWalk(Node* node, int row, const float mod, const float spacing)
    {
        if (node == nullptr)
            return;

        //calculate coords
        node->x = node->prelim + mod;
        node->y = row * spacing;

        Node* child = node->firstChild;
        if (child == nullptr) return;
        secondWalk(child, row + 1, mod + node->modifier, spacing);
        while (child->rightSibling != nullptr) {
            child = child->rightSibling;
            secondWalk(child, row + 1, mod + node->modifier, spacing);
        }
    }


    inline Node* getLeftNeighbour(const ImVector<Node*> nodeList, int idx)
    {
        Node* node = nodeList[idx];
        for (int n = 0; n < nodeList.size(); n++)
        {
            if ((nodeList[n]->column == node->column - 1) && (nodeList[n]->row == node->row))
                return nodeList[n];
        }

        return nullptr;
    }

    inline Node* getLeftSibling(const ImVector<Node*> nodeList, int idx)
    {
        Node* node = nodeList[idx];
        for (int n = 0; n < nodeList.size(); n++)
        {
            if ((nodeList[n]->column == node->column - 1) && (nodeList[n]->row == node->row) && (nodeList[n]->parent == node->parent))
                return nodeList[n];
        }

        return nullptr;
    }

    inline Node* getRightSibling(const ImVector<Node*> nodeList, int idx)
    {
        Node* node = nodeList[idx];
        for (int n = 0; n < nodeList.size(); n++)
        {
            if ((nodeList[n]->column == node->column + 1) && (nodeList[n]->row == node->row) && (nodeList[n]->parent == node->parent))
                return nodeList[n];
        }

        return nullptr;
    }

    inline void convertToNodeList(int row, ds::TwoThreeNode<int>* node, Node* parent, bool isFirstChild, ImVector<Node*>& nodeList)
    {
        if (node == nullptr) return;
        Node* tmp = new Node();
        tmp->k1 = node->k1;
        tmp->k2 = node->k2;
        tmp->n = node->n;
        tmp->row = row;
        tmp->parent = parent;
        if (parent != nullptr && isFirstChild) parent->firstChild = tmp;
        for (int i = 0; i < nodeList.size(); i++)
        {
            if (nodeList[i]->row == row) tmp->column++;
        }

        nodeList.push_back(tmp);
        if (node->left != nullptr) convertToNodeList(row + 1, node->left, tmp, true, nodeList);
        if (node->middle != nullptr) convertToNodeList(row + 1, node->middle, tmp, false, nodeList);
        if (node->right != nullptr) convertToNodeList(row + 1, node->right, tmp, false, nodeList);
    }

    inline void calculate(ImVector<Node*>& nodeList, ds::TwoThreeNode<int>* node, const float spacing, const float size)
    {
        if (node == nullptr) return;

        //convert tree structure to list so algorithm can run
        convertToNodeList(0, node, nullptr, false, nodeList);
        for (int i = 0; i < nodeList.size(); i++)
        {
            nodeList[i]->leftNeighbour = getLeftNeighbour(nodeList, i);
            nodeList[i]->leftSibling = getLeftSibling(nodeList, i);
            nodeList[i]->rightSibling = getRightSibling(nodeList, i);
        }
        Node* root = nullptr;
        for (int i = 0; i < nodeList.size(); i++)
        {
            if (nodeList[i]->row == 0 && nodeList[i]->column == 0)
            {
                root = nodeList[i];
                break;
            }
        }
        firstWalk(root, 0, spacing, size);
        secondWalk(root, 0, 0, spacing);
    }
}