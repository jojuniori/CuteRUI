#ifndef DOMINANTCOLOR_HPP
#define DOMINANTCOLOR_HPP

#include <Eigen/Core>

class DominantColor {
public:
    DominantColor(const unsigned char *data, int width, int height, int depth);
    ~DominantColor();
    std::vector<Eigen::Vector3i> find_dominant_colors(int count);
private:
    struct ColorNode;
    inline unsigned char color_at(int x, int y, int k);
    const Eigen::Vector3f color_at(int x, int y);
    inline unsigned char & class_at(int x, int y);
    void get_class_mean_cov(ColorNode * node);
    void partition_class(int nextid, ColorNode * node);
    ColorNode * get_max_eigenvalue_node();
private:
    ColorNode * root_;
    const unsigned char *data_;
    unsigned char *classes_;
    int width_;
    int height_;
    int depth_;
};

#endif
