#include "DominantColor.hpp"

#include <queue>
#include <iostream>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
struct DominantColor::ColorNode {
public:
    ColorNode() : left(nullptr), right(nullptr) {
    }

    ~ColorNode() {
        if (left != nullptr) {
            delete left;
        }

        if (right != nullptr) {
            delete right;
        }
    }

    std::vector<ColorNode *> get_leaves() {
        ColorNode * current = this;
        std::vector<ColorNode *> ret;
        std::queue<ColorNode *> queue;
        queue.push(current);

        while(queue.size() > 0) {
            current = queue.front();
            queue.pop();

            if(current->left != nullptr && current->right != nullptr) {
                queue.push(current->left);
                queue.push(current->right);
                continue;
            }

            ret.push_back(current);
        }

        return ret;
    }

    int get_next_classid() {
        ColorNode *current = this;
        int maxid = 0;
        std::queue<ColorNode*> queue;
        queue.push(current);

        while(queue.size() > 0) {
            current = queue.front();
            queue.pop();

            if(current->class_id > maxid)
                maxid = current->class_id;

            if(current->left != nullptr)
                queue.push(current->left);

            if(current->right)
                queue.push(current->right);
        }

        return maxid + 1;
    }

    std::vector<Eigen::Vector3i> get_dominant_colors() {
        std::vector<ColorNode*> leaves = get_leaves();
        std::vector<Eigen::Vector3i> ret;

        for(int i = 0;i < leaves.size(); i++) {
            Eigen::Vector3f mean = leaves[i]->mean;
            mean *= 255.0f;
            ret.push_back(mean.cast<int>());
        }

        return ret;
    }

    Eigen::Vector3f mean;
    Eigen::Matrix3f cov;
    unsigned char class_id;
    ColorNode *left;
    ColorNode *right;
};

DominantColor::DominantColor(const unsigned char *data, int width, int height, int depth)
        : data_(data), width_(width), height_(height), depth_(depth)
{
    classes_ = new unsigned char [width * height];
    memset(classes_, 1, static_cast<std::size_t>(width * height));
    root_ = new ColorNode();
    root_->class_id = 1;
}

DominantColor::~DominantColor() {
    delete[] classes_;
    delete root_;
}

unsigned char DominantColor::color_at(int x, int y, int k) {
    return data_[(y * width_ + x) * depth_ + k];
}

unsigned char & DominantColor::class_at(int x, int y) {
    return classes_[y * width_ + x];
}

const Eigen::Vector3f DominantColor::color_at(int x, int y) {
    int n = (y * width_ + x) * depth_;
    return Eigen::Vector3f{data_[n] / 255.0f, data_[n + 1] / 255.0f, data_[n + 2] / 255.0f};
}

void DominantColor::get_class_mean_cov(DominantColor::ColorNode *node) {
    const int cid = node->class_id;

    Eigen::Vector3f mean;
    mean.setZero();
    Eigen::Matrix3f cov;
    cov.setZero();

    // We start out with the average color
    double pixcount = 0;
    for(int y = 0;y < height_; y++) {
        for(int x = 0;x < width_;x++) {
            if(class_at(x, y) != cid)
                continue;
            mean += color_at(x, y);
            cov = cov + (color_at(x, y) * color_at(x, y).transpose());
            pixcount++;
        }
    }

    if (pixcount > 0) {
        cov = cov - (mean * mean.transpose()) / pixcount;
        mean *= 1.0 / pixcount;
    }

    // The node mean and covariance
    node->mean = mean;
    node->cov = cov;
}

void DominantColor::partition_class(int nextid, DominantColor::ColorNode *node) {
    unsigned char class_id = node->class_id;

    unsigned char id_left = nextid;
    unsigned char id_right = nextid + 1;

    Eigen::EigenSolver<Eigen::Matrix3f> es(node->cov);
    Eigen::Vector3cf eig = es.eigenvectors().col(0).cwiseAbs();
    auto comparison_value = eig.real().transpose() * node->mean;
    std::cout << "Partition, Eigs:" << eig << "CV: " << comparison_value << std::endl;

    node->left = new ColorNode();
    node->right = new ColorNode();

    node->left->class_id = id_left;
    node->right->class_id = id_right;

    for(int y = 0;y < height_; y++) {
        for(int x = 0; x < width_; x++) {
            if(class_at(x, y) != class_id)
                continue;

            auto this_value = eig.real().transpose() * color_at(x, y);

            if(this_value(0, 0) <= comparison_value(0, 0)) {
                class_at(x, y) = id_left;
            } else {
                class_at(x, y) = id_right;
            }
        }
    }
}

DominantColor::ColorNode *DominantColor::get_max_eigenvalue_node() {
    double max_eigen = -1;
    //Eigen::Matrix eigenvalues, eigenvectors;

    std::queue<ColorNode*> queue;
    queue.push(root_);

    ColorNode * ret = root_;
    if(!root_->left && !root_->right)
        return root_;

    while(queue.size() > 0) {
        ColorNode *node = queue.front();
        queue.pop();

        if(node->left && node->right) {
            queue.push(node->left);
            queue.push(node->right);
            continue;
        }

        //cv::eigen(node->cov, eigenvalues, eigenvectors);
        auto x = node->cov.eigenvalues();
        std::cout << "Max, Eigs:" << node->cov.eigenvalues() << "CV: " << node->cov.eigenvalues() << std::endl;
        double val = node->cov.eigenvalues().x().real();//eigenvalues.at<double>(0);
        if(val > max_eigen) {
            max_eigen = val;
            ret = node;
        }
    }

    return ret;
}

std::vector<Eigen::Vector3i> DominantColor::find_dominant_colors(int count) {
    ColorNode *next = root_;
    get_class_mean_cov(next);
    for(int i=0;i<count-1;i++) {
        next = get_max_eigenvalue_node();
        partition_class(root_->get_next_classid(), next);
        get_class_mean_cov(next->left);
        get_class_mean_cov(next->right);
    }

    std::vector<Eigen::Vector3i> colors = root_->get_dominant_colors();

    return colors;
}
