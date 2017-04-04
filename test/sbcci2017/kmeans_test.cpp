#include "../catch.hpp"
#include "kmeans_data_oriented_design.h"
#include "kmeans_object_oriented_design.h"
#include "kmeans_hybrid.h"
#include "metrics.h"
#include "../../ophidian/parser/Def.h"

#include <chrono>
#include <memory>

bool point_comparison(const ophidian::geometry::Point & point1, const ophidian::geometry::Point & point2){
    return boost::geometry::equals(point1, point2);
}

bool cluster_comparison(const std::vector<ophidian::geometry::Point> & cluster1, const std::vector<ophidian::geometry::Point> & cluster2) {
    if (cluster1.size() != cluster2.size()) {
        return false;
    }
    return std::is_permutation(cluster1.begin(), cluster1.end(), cluster2.begin(), point_comparison);
}

TEST_CASE("kmeans/ initialize random clusters","[test][kmeans]") {
    ophidian::geometry::Point chipOrigin(1.0, 1.0);
    ophidian::geometry::Point chipBondary(100.0, 100.0);

    ophidian::KmeansDataOrientedDesign kmeansDOD (chipOrigin, chipBondary, 25);
    REQUIRE(kmeansDOD.clusters_.size() == 25);
    REQUIRE(kmeansDOD.clusterCenters_.size() == 25);

    for(auto p : kmeansDOD.clusterCenters_){
        REQUIRE(p.x() >= chipOrigin.x());
        REQUIRE(p.y() >= chipOrigin.y());
        REQUIRE(p.x() <= chipBondary.x());
        REQUIRE(p.y() <= chipBondary.y());
    }
}

TEST_CASE("kmeans/ basic test structure","[test][kmeans]") {
   using Point = ophidian::geometry::Point;

   Point a(5,7);
   ophidian::ClusterOOD cluster(a);
   REQUIRE( cluster.center().x() == 5 );
   REQUIRE( cluster.center().y() == 7 );

   cluster.center(Point(8,9));
   REQUIRE( cluster.center().x() == 8 );
   REQUIRE( cluster.center().y() == 9 );

   Point ff(1,2);
   ophidian::FlipFlop element(ff);
   REQUIRE( element.position().x() == 1);
   REQUIRE( element.position().y() == 2);
   element.setPosition(Point(3,4));
   REQUIRE( element.position().x() == 3);
   REQUIRE( element.position().y() == 4);

   cluster.insertElement(element);
   REQUIRE(cluster.size() == 1);

   ophidian::FlipFlop ff_2 = cluster.elements().front();
   REQUIRE(ff_2.position().x() == 3);
   REQUIRE(ff_2.position().y()== 4);

}

TEST_CASE("kmeans/ initialize clusters by vector (DOD-sequential)","[test][kmeans]") {
    std::vector<ophidian::geometry::Point> flip_flop_positions = {
       {1, 1},
       {3, 2},
       {2, 1},
       {1, 3},
       {7, 2},
       {7, 1},
       {9, 1},
       {9, 3},
       {1, 9},
       {3, 7},
       {1, 8},
       {2, 7},
       {7, 7},
       {7, 9},
       {7, 8},
       {8, 7}
   };
   std::vector<ophidian::geometry::Point> initial_centers = {
        {2, 2},
        {8, 2},
        {2, 8},
        {8, 8}
   };
   std::vector<ophidian::geometry::Point> expected_centers = {
        {1.75, 1.75},
        {8, 1.75},
        {1.75, 7.75},
        {7.25, 7.75}
   };
   std::vector<std::vector<ophidian::geometry::Point>> expected_clusters = {
        {{1, 1}, {3, 2}, {2, 1}, {1, 3}},
        {{7, 2}, {7, 1}, {9, 1}, {9, 3}},
        {{1, 9}, {3, 7}, {1, 8}, {2, 7}},
        {{7, 7}, {7, 9}, {7, 8}, {8, 7}}
   };

   ophidian::KmeansDataOrientedDesign kmeansDOD(initial_centers);
   REQUIRE(kmeansDOD.clusters_.size() == initial_centers.size());

   kmeansDOD.cluster_registers(flip_flop_positions, 1);

   REQUIRE(kmeansDOD.clusterCenters_.size() == expected_centers.size());
   REQUIRE(std::is_permutation(kmeansDOD.clusterCenters_.begin(), kmeansDOD.clusterCenters_.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansDOD.clusterElements_.size() == expected_clusters.size());
   REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), expected_clusters.begin(), cluster_comparison));

}

TEST_CASE("kmeans/ initialize clusters by vector (DOD-parallel)","[test][kmeans]") {
    std::vector<ophidian::geometry::Point> flip_flop_positions = {
       {1, 1},
       {3, 2},
       {2, 1},
       {1, 3},
       {7, 2},
       {7, 1},
       {9, 1},
       {9, 3},
       {1, 9},
       {3, 7},
       {1, 8},
       {2, 7},
       {7, 7},
       {7, 9},
       {7, 8},
       {8, 7}
   };
   std::vector<ophidian::geometry::Point> initial_centers = {
        {2, 2},
        {8, 2},
        {2, 8},
        {8, 8}
   };
   std::vector<ophidian::geometry::Point> expected_centers = {
        {1.75, 1.75},
        {8, 1.75},
        {1.75, 7.75},
        {7.25, 7.75}
   };
   std::vector<std::vector<ophidian::geometry::Point>> expected_clusters = {
        {{1, 1}, {3, 2}, {2, 1}, {1, 3}},
        {{7, 2}, {7, 1}, {9, 1}, {9, 3}},
        {{1, 9}, {3, 7}, {1, 8}, {2, 7}},
        {{7, 7}, {7, 9}, {7, 8}, {8, 7}}
   };

   ophidian::KmeansDataOrientedDesign kmeansDOD(initial_centers);
   REQUIRE(kmeansDOD.clusters_.size() == initial_centers.size());

   kmeansDOD.cluster_registers_parallel(flip_flop_positions, 1);

   REQUIRE(kmeansDOD.clusterCenters_.size() == expected_centers.size());
   REQUIRE(std::is_permutation(kmeansDOD.clusterCenters_.begin(), kmeansDOD.clusterCenters_.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansDOD.clusterElements_.size() == expected_clusters.size());
   REQUIRE(std::is_permutation(kmeansDOD.clusterElements_.begin(), kmeansDOD.clusterElements_.end(), expected_clusters.begin(), cluster_comparison));

}

TEST_CASE("kmeans/ initialize clusters by vector (OOD-sequential)","[test][kmeans]") {
    std::vector<ophidian::geometry::Point> flip_flop_positions = {
       {1, 1},
       {3, 2},
       {2, 1},
       {1, 3},
       {7, 2},
       {7, 1},
       {9, 1},
       {9, 3},
       {1, 9},
       {3, 7},
       {1, 8},
       {2, 7},
       {7, 7},
       {7, 9},
       {7, 8},
       {8, 7}
   };
   std::vector<ophidian::geometry::Point> initial_centers = {
        {2, 2},
        {8, 2},
        {2, 8},
        {8, 8}
   };
   std::vector<ophidian::geometry::Point> expected_centers = {
        {1.75, 1.75},
        {8, 1.75},
        {1.75, 7.75},
        {7.25, 7.75}
   };
   std::vector<std::vector<ophidian::geometry::Point>> expected_clusters = {
        {{1, 1}, {3, 2}, {2, 1}, {1, 3}},
        {{7, 2}, {7, 1}, {9, 1}, {9, 3}},
        {{1, 9}, {3, 7}, {1, 8}, {2, 7}},
        {{7, 7}, {7, 9}, {7, 8}, {8, 7}}
   };

   ophidian::KmeansObjectOrientedDesign kmeansOOD(initial_centers);
   REQUIRE(kmeansOOD.clusters_.size() == initial_centers.size());

   std::vector<ophidian::FlipFlop> flip_flops;
   flip_flops.reserve(flip_flop_positions.size());
   for(auto p : flip_flop_positions){
       flip_flops.push_back(ophidian::FlipFlop(p));
   }
   kmeansOOD.cluster_registers(flip_flops, 1);

   REQUIRE(kmeansOOD.clusters_.size() == expected_centers.size());
   std::vector<ophidian::geometry::Point> clusters_positions;
   clusters_positions.reserve(kmeansOOD.clusters_.size());
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       clusters_positions.push_back(cluster.center());
   }
   REQUIRE(std::is_permutation(clusters_positions.begin(), clusters_positions.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansOOD.clusters_.size() == expected_clusters.size());
   std::vector<std::vector<ophidian::geometry::Point>> cluster_return;
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       std::vector<ophidian::geometry::Point> elements_positions;
       elements_positions.reserve(cluster.elements().size());
       for(ophidian::FlipFlop ff : cluster.elements()){
           elements_positions.push_back(ff.position());
       }
       cluster_return.push_back(elements_positions);
   }
   REQUIRE(std::is_permutation(cluster_return.begin(), cluster_return.end(), expected_clusters.begin(), cluster_comparison));
}

TEST_CASE("kmeans/ initialize clusters by vector (OOD-sequential) using Rtree","[test][kmeans]") {
    std::vector<ophidian::geometry::Point> flip_flop_positions = {
       {1, 1},
       {3, 2},
       {2, 1},
       {1, 3},
       {7, 2},
       {7, 1},
       {9, 1},
       {9, 3},
       {1, 9},
       {3, 7},
       {1, 8},
       {2, 7},
       {7, 7},
       {7, 9},
       {7, 8},
       {8, 7}
   };
   std::vector<ophidian::geometry::Point> initial_centers = {
        {2, 2},
        {8, 2},
        {2, 8},
        {8, 8}
   };
   std::vector<ophidian::geometry::Point> expected_centers = {
        {1.75, 1.75},
        {8, 1.75},
        {1.75, 7.75},
        {7.25, 7.75}
   };
   std::vector<std::vector<ophidian::geometry::Point>> expected_clusters = {
        {{1, 1}, {3, 2}, {2, 1}, {1, 3}},
        {{7, 2}, {7, 1}, {9, 1}, {9, 3}},
        {{1, 9}, {3, 7}, {1, 8}, {2, 7}},
        {{7, 7}, {7, 9}, {7, 8}, {8, 7}}
   };

   ophidian::KmeansObjectOrientedDesign kmeansOOD(initial_centers);
   REQUIRE(kmeansOOD.clusters_.size() == initial_centers.size());

   std::vector<ophidian::FlipFlop> flip_flops;
   flip_flops.reserve(flip_flop_positions.size());
   for(auto p : flip_flop_positions){
       flip_flops.push_back(ophidian::FlipFlop(p));
   }
   kmeansOOD.cluster_registers_with_rtree_paralel(flip_flops, 1);

   REQUIRE(kmeansOOD.clusters_.size() == expected_centers.size());
   std::vector<ophidian::geometry::Point> clusters_positions;
   clusters_positions.reserve(kmeansOOD.clusters_.size());
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       clusters_positions.push_back(cluster.center());
   }
   REQUIRE(std::is_permutation(clusters_positions.begin(), clusters_positions.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansOOD.clusters_.size() == expected_clusters.size());
   std::vector<std::vector<ophidian::geometry::Point>> cluster_return;
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       std::vector<ophidian::geometry::Point> elements_positions;
       elements_positions.reserve(cluster.elements().size());
       for(ophidian::FlipFlop ff : cluster.elements()){
           elements_positions.push_back(ff.position());
       }
       cluster_return.push_back(elements_positions);
   }
   REQUIRE(std::is_permutation(cluster_return.begin(), cluster_return.end(), expected_clusters.begin(), cluster_comparison));
}

TEST_CASE("kmeans/ initialize clusters by vector (OOD-parallel)","[test][kmeans]") {
    std::vector<ophidian::geometry::Point> flip_flop_positions = {
       {1, 1},
       {3, 2},
       {2, 1},
       {1, 3},
       {7, 2},
       {7, 1},
       {9, 1},
       {9, 3},
       {1, 9},
       {3, 7},
       {1, 8},
       {2, 7},
       {7, 7},
       {7, 9},
       {7, 8},
       {8, 7}
   };
   std::vector<ophidian::geometry::Point> initial_centers = {
        {2, 2},
        {8, 2},
        {2, 8},
        {8, 8}
   };
   std::vector<ophidian::geometry::Point> expected_centers = {
        {1.75, 1.75},
        {8, 1.75},
        {1.75, 7.75},
        {7.25, 7.75}
   };
   std::vector<std::vector<ophidian::geometry::Point>> expected_clusters = {
        {{1, 1}, {3, 2}, {2, 1}, {1, 3}},
        {{7, 2}, {7, 1}, {9, 1}, {9, 3}},
        {{1, 9}, {3, 7}, {1, 8}, {2, 7}},
        {{7, 7}, {7, 9}, {7, 8}, {8, 7}}
   };

   ophidian::KmeansObjectOrientedDesign kmeansOOD(initial_centers);
   REQUIRE(kmeansOOD.clusters_.size() == initial_centers.size());

   std::vector<ophidian::FlipFlop> flip_flops;
   flip_flops.reserve(flip_flop_positions.size());
   for(auto p : flip_flop_positions){
       flip_flops.push_back(ophidian::FlipFlop(p));
   }
   kmeansOOD.cluster_registers_paralel(flip_flops, 1);

   REQUIRE(kmeansOOD.clusters_.size() == expected_centers.size());
   std::vector<ophidian::geometry::Point> clusters_positions;
   clusters_positions.reserve(kmeansOOD.clusters_.size());
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       clusters_positions.push_back(cluster.center());
   }
   REQUIRE(std::is_permutation(clusters_positions.begin(), clusters_positions.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansOOD.clusters_.size() == expected_clusters.size());
   std::vector<std::vector<ophidian::geometry::Point>> cluster_return;
   for(ophidian::ClusterOOD cluster : kmeansOOD.clusters_){
       std::vector<ophidian::geometry::Point> elements_positions;
       elements_positions.reserve(cluster.elements().size());
       for(ophidian::FlipFlop ff : cluster.elements()){
           elements_positions.push_back(ff.position());
       }
       cluster_return.push_back(elements_positions);
   }
   REQUIRE(std::is_permutation(cluster_return.begin(), cluster_return.end(), expected_clusters.begin(), cluster_comparison));
}

TEST_CASE("kmeans/ initialize clusters by vector (Hybrid)","[test][kmeans][hybrid]") {
    std::vector<ophidian::geometry::Point> flip_flop_positions = {
       {1, 1},
       {3, 2},
       {2, 1},
       {1, 3},
       {7, 2},
       {7, 1},
       {9, 1},
       {9, 3},
       {1, 9},
       {3, 7},
       {1, 8},
       {2, 7},
       {7, 7},
       {7, 9},
       {7, 8},
       {8, 7}
   };
   std::vector<ophidian::geometry::Point> initial_centers = {
        {2, 2},
        {8, 2},
        {2, 8},
        {8, 8}
   };
   std::vector<ophidian::geometry::Point> expected_centers = {
        {1.75, 1.75},
        {8, 1.75},
        {1.75, 7.75},
        {7.25, 7.75}
   };
   std::vector<std::vector<ophidian::geometry::Point>> expected_clusters = {
        {{1, 1}, {3, 2}, {2, 1}, {1, 3}},
        {{7, 2}, {7, 1}, {9, 1}, {9, 3}},
        {{1, 9}, {3, 7}, {1, 8}, {2, 7}},
        {{7, 7}, {7, 9}, {7, 8}, {8, 7}}
   };

   ophidian::KmeansHybrid kmeansH(initial_centers);
   REQUIRE(kmeansH.clusters_.size() == initial_centers.size());

   kmeansH.cluster_registers_parallel(flip_flop_positions, 1);

   REQUIRE(kmeansH.clusters_.size() == expected_centers.size());
   std::vector<ophidian::geometry::Point> clusters_positions;
   clusters_positions.reserve(kmeansH.clusters_.size());
   for(ophidian::ClusterHybrid cluster : kmeansH.clusters_){
       clusters_positions.push_back(cluster.center());
   }
   REQUIRE(std::is_permutation(clusters_positions.begin(), clusters_positions.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansH.clusters_.size() == expected_clusters.size());
   std::vector<std::vector<ophidian::geometry::Point>> cluster_return;
   for(ophidian::ClusterHybrid cluster : kmeansH.clusters_){
       std::vector<ophidian::geometry::Point> elements_positions;
       elements_positions.reserve(cluster.elements().size());
       for(ophidian::FlipFlop ff : cluster.elements()){
           elements_positions.push_back(ff.position());
       }
       cluster_return.push_back(elements_positions);
   }
   REQUIRE(std::is_permutation(cluster_return.begin(), cluster_return.end(), expected_clusters.begin(), cluster_comparison));

}

TEST_CASE("kmeans/ initialize clusters by vector (Hybrid) using rtree","[test][kmeans][hybrid][rtree]") {
    std::vector<ophidian::geometry::Point> flip_flop_positions = {
       {1, 1},
       {3, 2},
       {2, 1},
       {1, 3},
       {7, 2},
       {7, 1},
       {9, 1},
       {9, 3},
       {1, 9},
       {3, 7},
       {1, 8},
       {2, 7},
       {7, 7},
       {7, 9},
       {7, 8},
       {8, 7}
   };
   std::vector<ophidian::geometry::Point> initial_centers = {
        {2, 2},
        {8, 2},
        {2, 8},
        {8, 8}
   };
   std::vector<ophidian::geometry::Point> expected_centers = {
        {1.75, 1.75},
        {8, 1.75},
        {1.75, 7.75},
        {7.25, 7.75}
   };
   std::vector<std::vector<ophidian::geometry::Point>> expected_clusters = {
        {{1, 1}, {3, 2}, {2, 1}, {1, 3}},
        {{7, 2}, {7, 1}, {9, 1}, {9, 3}},
        {{1, 9}, {3, 7}, {1, 8}, {2, 7}},
        {{7, 7}, {7, 9}, {7, 8}, {8, 7}}
   };

   ophidian::KmeansHybrid kmeansH(initial_centers);
   REQUIRE(kmeansH.clusters_.size() == initial_centers.size());

   kmeansH.cluster_registers_with_rtree_parallel(flip_flop_positions, 10);

   REQUIRE(kmeansH.clusters_.size() == expected_centers.size());
   std::vector<ophidian::geometry::Point> clusters_positions;
   clusters_positions.reserve(kmeansH.clusters_.size());
   for(ophidian::ClusterHybrid cluster : kmeansH.clusters_){
       clusters_positions.push_back(cluster.center());
   }
   REQUIRE(std::is_permutation(clusters_positions.begin(), clusters_positions.end(), expected_centers.begin(), point_comparison));

   REQUIRE(kmeansH.clusters_.size() == expected_clusters.size());
   std::vector<std::vector<ophidian::geometry::Point>> cluster_return;
   for(ophidian::ClusterHybrid cluster : kmeansH.clusters_){
       std::vector<ophidian::geometry::Point> elements_positions;
       elements_positions.reserve(cluster.elements().size());
       for(ophidian::FlipFlop ff : cluster.elements()){
           elements_positions.push_back(ff.position());
       }
       cluster_return.push_back(elements_positions);
   }
   REQUIRE(std::is_permutation(cluster_return.begin(), cluster_return.end(), expected_clusters.begin(), cluster_comparison));

}


