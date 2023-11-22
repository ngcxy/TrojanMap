#include "trojanmap.h"

// ==============================
// Helper Function
// ==============================
bool IsPrefixMatched(const std::string& prefix, const std::string& target)
{

  // check only if prefix is provided && prefix.size <= target
  if(!prefix.empty() && prefix.length()<= target.length())
  {
    // compare prefix with target  (not case sensitive)
    for(int i=0; i<prefix.size(); i++)
    {
      if(std::tolower(prefix[i]) != std::tolower(target[i]))
        return false;
    }

    // if prefix is matched, return true
    return true;
  }

  // invalid prefix is provided
  else
    return false;
}

// Dijkstra Algorithm to find the destination id2
bool TrojanMap::DijkstraFound(std::priority_queue<std::pair<double, std::string>,
            std::vector<std::pair<double, std::string>>,
            std::greater<std::pair<double, std::string>>>& q,
            std::unordered_map<std::string,
            std::pair<bool, std::pair< double, std::string >>>& mark,
            std::string id2){

  // get the shortest edge <distance, id>
  std::pair<double, std::string> current = q.top();

    // destination found
    if(current.second == id2){
      return true;
    }

    q.pop();
    mark[current.second].first = true;

    // update distance
    std::vector<std::string> current_neighbors = data[current.second].neighbors;
    for(auto i : current_neighbors){
      double temp_distance = current.first + CalculateDistance(current.second, i);
      // update previously unvisited node
      if(mark.find(i) == mark.end()){
        q.push(std::pair<double, std::string>(temp_distance, i));
        mark.insert(std::pair< std::string, std::pair< bool, std::pair<double, std::string> > >
              (i, std::pair< bool, std::pair<double, std::string> >(false, std::pair<double, std::string>(temp_distance, current.second))));
      }
      // update visited node if distance becomes shorter
      else if(mark[i].second.first > temp_distance){
        mark[i].second.first = temp_distance;
        mark[i].second.second = current.second;
        q.push(std::pair<double, std::string>(temp_distance, i));
      }
    }

    //remove unwanted tops
    while(!q.empty() && mark[q.top().second].first)
      q.pop();
    return false;
}

bool dfsCycle(  std::string currId, std::string parentId,
                std::unordered_map<std::string, Node>& data, std::vector<std::string>& subgraph,
                std::unordered_set<std::string>& visited, std::stack<std::string>& path)
{
    // curr node has been visited before, a cycle is detected
    if(visited.find(currId) != visited.end())
        return true;

    // 1.current node has not been visited, continue dfs the neighbor of current node
    // 2.record the path
    visited.insert(currId);
    path.push(currId);

    for(const std::string& neighborId : data[currId].neighbors)
    {
        bool isInSubgraph = std::find(subgraph.begin(), subgraph.end(), neighborId) != subgraph.end();
        bool isNotParentNode = neighborId != parentId;

        if(isNotParentNode && isInSubgraph){
            // if a sub-path contains a path, return immediately
            if(dfsCycle(neighborId, currId, data, subgraph, visited, path))
            {
                return true;
            }
        }
    }

    // no cycle is detected, all path has been visited, erase the path
    path.pop();
    return false;
}

//-----------------------------------------------------
// TODO: Students should implement the following:
//-----------------------------------------------------
/**
 * GetLat: Get the latitude of a Node given its id. If id does not exist, return
 * -1.
 *
 * @param  {std::string} id : location id
 * @return {double}         : latitude
 */
double TrojanMap::GetLat(const std::string &id) {
  auto iter = data.find(id);
  // check if found
  if(iter != data.end())
    return iter->second.lat;
  else
    return -1;
}

/**
 * GetLon: Get the longitude of a Node given its id. If id does not exist,
 * return -1.
 *
 * @param  {std::string} id : location id
 * @return {double}         : longitude
 */
double TrojanMap::GetLon(const std::string &id) {
  auto iter = data.find(id);
  // check if found
  if(iter != data.end())
    return iter->second.lon;
  else
    return -1;
}

/**
 * GetName: Get the name of a Node given its id. If id does not exist, return
 * "NULL".
 *
 * @param  {std::string} id : location id
 * @return {std::string}    : name
 */
std::string TrojanMap::GetName(const std::string &id) {
  auto iter = data.find(id);
  // check if found
  if(iter != data.end())
    return iter->second.name;
  else
    return "NULL";
}

/**
 * GetNeighborIDs: Get the neighbor ids of a Node. If id does not exist, return
 * an empty vector.
 *
 * @param  {std::string} id            : location id
 * @return {std::vector<std::string>}  : neighbor ids
 */
std::vector<std::string> TrojanMap::GetNeighborIDs(const std::string &id) {
  auto iter = data.find(id);
  // check if found
  if(iter != data.end())
    return iter->second.neighbors;
  else
    return {};
}

/**
 * GetID: Given a location name, return the id.
 * If the node does not exist, return an empty string.
 * The location name must be unique, which means there is only one node with the name.
 *
 * @param  {std::string} name          : location name
 * @return {std::string}               : id
 */
std::string TrojanMap::GetID(const std::string &name) {
  std::string res = "";

  // loop through all nodes to find the one with expected name
  for(auto& p : data){
    // node name must be unique, empty is not a valid location name
    if(!p.second.name.empty() && p.second.name == name)
    {
      // return ID, which is the key of the pair
      res = p.first;
      break;
    }
  }
  return res;
}

/**
 * GetPosition: Given a location name, return the position. If id does not
 * exist, return (-1, -1).
 *
 * @param  {std::string} name          : location name
 * @return {std::pair<double,double>}  : (lat, lon)
 */
std::pair<double, double> TrojanMap::GetPosition(std::string name) {
  std::pair<double, double> results(-1, -1);
  std::string id = GetID(name);
  if(!id.empty()){
    results.first = GetLat(id);
    results.second= GetLon(id);
  }
  return results;
}

/**
 * CalculateEditDistance: Calculate edit distance between two location names
 * @param  {std::string} a          : first string
 * @param  {std::string} b          : second string
 * @return {int}                    : edit distance between two strings
 */
int TrojanMap::CalculateEditDistance(std::string a, std::string b) {
    int la = a.length(), lb = b.length();

    // at least one is empty
    if (la*lb == 0)
        return (la+lb);

    // dp vector
    std::vector<std::vector<int>> dp(la+1, std::vector<int>(lb+1));
    // base case
    for (int i=0; i<la+1; i++)
        dp[i][0]=i;
    for (int i=1; i<lb+1; i++)
        dp[0][i]=i;

    // recurrence
    for (int i=1; i<la+1; i++){
        for (int j=1; j<lb+1; j++){
            dp[i][j] = std::min({dp[i-1][j]+1 , dp[i][j-1]+1 , (a[i-1]==b[j-1]) ? dp[i-1][j-1] : dp[i-1][j-1]+1});
        }
    }

    return dp[la][lb];
}

/**
 * FindClosestName: Given a location name, return the name with the smallest edit
 * distance.
 *
 * @param  {std::string} name          : location name
 * @return {std::string} tmp           : the closest name
 */
std::string TrojanMap::FindClosestName(std::string name) {
    int min_distance = INT_MAX;
    std::string tmp;

    for(auto& p : data){
        std::string p_name = p.second.name;
        // return immediately if identical
        if (p_name == name ) {
            return p_name;
        }
        // calculate the edit distance
        int distance = CalculateEditDistance(name, p_name);
        if(!name.empty() && distance<min_distance){
            min_distance = distance;
            tmp = p_name;
        }
    }
    return tmp;
}

/**
 * Autocomplete: Given a parital name return all the possible locations with
 * partial name as the prefix. The function should be case-insensitive.
 *
 * @param  {std::string} name          : partial name
 * @return {std::vector<std::string>}  : a vector of full names
 */
std::vector<std::string> TrojanMap::Autocomplete(std::string name) {
  std::vector<std::string> results;

  // TODO: loop through the map data to find all possible location names with matched prefix
  for(const auto& tmp : data)
  {
    // 1. a location must has a name;  2. location's prefix must be matched with <name>
    const std::string& location = tmp.second.name;
    if(!location.empty() && IsPrefixMatched(name, location))
      results.push_back(location);
  }
  return results;
}

/**
 * GetAllCategories: Return all the possible unique location categories, i.e.
 * there should be no duplicates in the output.
 *
 * @return {std::vector<std::string>}  : all unique location categories
 */
std::vector<std::string> TrojanMap::GetAllCategories() {
    std::set<std::string> unique_Cats;
    for(auto& ptr : data){
        for(auto category : ptr.second.attributes){
            unique_Cats.insert(category);
        }
    }
    return std::vector<std::string>(unique_Cats.begin(), unique_Cats.end());
}

/**
 * GetAllLocationsFromCategory: Return all the locations of the input category (i.e.
 * 'attributes' in data.csv). If there is no location of that category, return
 * (-1, -1). The function should be case-insensitive.
 *
 * @param  {std::string} category         : category name (attribute)
 * @return {std::vector<std::string>}     : ids
 */
std::vector<std::string> TrojanMap::GetAllLocationsFromCategory(
    std::string category) {
    std::vector<std::string> res;

    for(auto& ptr : data){
        auto& attrSet = ptr.second.attributes;
        if(!attrSet.empty())
        {
            auto iter = attrSet.find(category);
            if(iter != attrSet.end()){
                res.push_back(ptr.second.id);
            }
        }
    }

    if(res.empty()){
        res.push_back("-1");
        res.push_back("-1");
    }

    return res;
}

/**
 * GetLocationRegex: Given the regular expression of a location's name, your
 * program should first check whether the regular expression is valid, and if so
 * it returns all locations that match that regular expression.
 *
 * @param  {std::regex} location name      : the regular expression of location
 * names
 * @return {std::vector<std::string>}     : ids
 */
std::vector<std::string> TrojanMap::GetLocationRegex(std::regex location) {

    std::vector<std::string> res;

    // if invalid regex or if no match
    for(auto& node : data)
    {
        const std::string& name = node.second.name;
        if(std::regex_match(name ,location)){
            res.push_back(node.first);
        }
    }

    return res;
}

/**
 * CalculateDistance: Get the distance between 2 nodes.
 * We have provided the code for you. Please do not need to change this function.
 * You can use this function to calculate the distance between 2 nodes.
 * The distance is in mile.
 * The distance is calculated using the Haversine formula.
 * https://en.wikipedia.org/wiki/Haversine_formula
 *
 * @param  {std::string} a  : a_id
 * @param  {std::string} b  : b_id
 * @return {double}  : distance in mile
 */
double TrojanMap::CalculateDistance(const std::string &a_id,
                                    const std::string &b_id) {
  // Do not change this function
  Node a = data[a_id];
  Node b = data[b_id];
  double dlon = (b.lon - a.lon) * M_PI / 180.0;
  double dlat = (b.lat - a.lat) * M_PI / 180.0;
  double p = pow(sin(dlat / 2), 2.0) + cos(a.lat * M_PI / 180.0) *
                                           cos(b.lat * M_PI / 180.0) *
                                           pow(sin(dlon / 2), 2.0);
  double c = 2 * asin(std::min(1.0, sqrt(p)));
  return c * 3961;
}

/**
 * CalculatePathLength: Calculates the total path length for the locations
 * inside the vector.
 * We have provided the code for you. Please do not need to change this function.
 *
 * @param  {std::vector<std::string>} path : path
 * @return {double}                        : path length
 */
double TrojanMap::CalculatePathLength(const std::vector<std::string> &path) {
  // Do not change this function
  double sum = 0;
  for (int i = 0; i < int(path.size()) - 1; i++) {
    sum += CalculateDistance(path[i], path[i + 1]);
  }
  return sum;
}

/**
 * CalculateShortestPath_Dijkstra: Given 2 locations, return the shortest path
 * which is a list of id. Hint: Use priority queue.
 *
 * @param  {std::string} location1_name     : start
 * @param  {std::string} location2_name     : goal
 * @return {std::vector<std::string>}       : path
 */
std::vector<std::string> TrojanMap::CalculateShortestPath_Dijkstra(
    std::string location1_name, std::string location2_name) {
  std::vector<std::string> path;        //the return vector
  std::string id1 = GetID(location1_name);
  std::string id2 = GetID(location2_name);

  //priority_queue: pair(distance, id)
  std::priority_queue< std::pair<double, std::string>, std::vector< std::pair<double, std::string>>,
    std::greater<std::pair<double, std::string>>> q;

  //< id, < visited, < distance, last >>>
  std::unordered_map< std::string, std::pair< bool, std::pair< double, std::string >>> mark;

  //insert the source node
  q.push(std::pair<double, std::string>(0, id1));
  mark.insert(std::pair< std::string, std::pair< bool, std::pair<double, std::string>>>
            (id1, std::pair< bool, std::pair<double, std::string> >(false, std::pair<double, std::string>(0, "0"))));

  // run Dijkstra Algorithm to find the path
  while(!q.empty()){
    if (DijkstraFound(q, mark, id2))
      break;
  }

  // not found
  if(q.empty())
    return path;
  // build the path
  std::string temp = q.top().second;
  while(temp != id1)
  {
    path.push_back(temp);
    temp = mark[temp].second.second;
  }
  path.push_back(temp);
  reverse(path.begin(), path.end());
  return path;
}

/**
 * CalculateShortestPath_Bellman_Ford: Given 2 locations, return the shortest
 * path which is a list of id. Hint: Do the early termination when there is no
 * change on distance.
 *
 * @param  {std::string} location1_name     : start
 * @param  {std::string} location2_name     : goal
 * @return {std::vector<std::string>}       : path
 */
std::vector<std::string> TrojanMap::CalculateShortestPath_Bellman_Ford(
    std::string location1_name, std::string location2_name) {
  std::string id1 = GetID(location1_name);
  std::string id2 = GetID(location2_name);

  //1D method in lec8 slides
  std::unordered_map<std::string, double> distances;    // map for updating distances
  std::unordered_map<std::string, std::string> lasts;   //map for the last

  //initialize
  for(auto i : data){
    distances.insert(std::pair<std::string, double>(i.first, INT_MAX));
    lasts.insert(std::pair<std::string, std::string>(i.first, ""));
  }
  distances[id1] = 0;

  for (int i = 0; i < data.size() - 1; i++){
    bool flag = false;  // flag for updating
    for (auto v : data){
      std::string cur = v.first;
      if(distances[cur]==INT_MAX)
        continue;
      for(auto neighbor : v.second.neighbors){
        // update distance for neighbours
        double newdistance = distances[cur] + CalculateDistance(neighbor, cur);
        if(distances[neighbor] > newdistance){
          distances[neighbor] = newdistance;
          lasts[neighbor] = cur;
          flag = true;
        }
      }
    }
    //if no changes
    if(!flag)
      break;
  }

  // build the path
  std::vector<std::string> path;
  path.push_back(id2);
  std::string temp = lasts[id2];
  while(temp != id1){
    path.push_back(temp);
    temp = lasts[temp];
  }
  path.push_back(id1);
  reverse(path.begin(), path.end());

  return path;
}

/**
 * Traveling salesman problem: Given a list of locations, return the shortest
 * path which visit all the places and back to the start point.
 *
 * @param  {std::vector<std::string>} input : a list of locations needs to visit
 * @return {std::pair<double, std::vector<std::vector<std::string>>} : a pair of total distance and the all the progress to get final path,
 *                                                                      for example: {10.3, {{0, 1, 2, 3, 4, 0}, {0, 1, 2, 3, 4, 0}, {0, 4, 3, 2, 1, 0}}},
 *                                                                      where 10.3 is the total distance,
 *                                                                      and the first vector is the path from 0 and travse all the nodes and back to 0,
 *                                                                      and the second vector is the path shorter than the first one,
 *                                                                      and the last vector is the shortest path.
 */
// Please use brute force to implement this function, ie. find all the permutations.
std::pair<double, std::vector<std::vector<std::string>>> TrojanMap::TravelingTrojan_Brute_force(
                                    std::vector<std::string> location_ids) {
  std::pair<double, std::vector<std::vector<std::string>>> records;
  return records;
}

// Please use backtracking to implement this function
std::pair<double, std::vector<std::vector<std::string>>> TrojanMap::TravelingTrojan_Backtracking(
                                    std::vector<std::string> location_ids) {
  std::pair<double, std::vector<std::vector<std::string>>> records;
  return records;
}

// Hint: https://en.wikipedia.org/wiki/2-opt
std::pair<double, std::vector<std::vector<std::string>>> TrojanMap::TravelingTrojan_2opt(
      std::vector<std::string> location_ids){
  std::pair<double, std::vector<std::vector<std::string>>> records;
  return records;
}

// This is optional
std::pair<double, std::vector<std::vector<std::string>>> TrojanMap::TravelingTrojan_3opt(
      std::vector<std::string> location_ids){
  std::pair<double, std::vector<std::vector<std::string>>> records;
  return records;
}

/**
 * Given CSV filename, it read and parse locations data from CSV file,
 * and return locations vector for topological sort problem.
 * We have provided the code for you. Please do not need to change this function.
 * Example:
 *   Input: "topologicalsort_locations.csv"
 *   File content:
 *    Name
 *    Ralphs
 *    KFC
 *    Chick-fil-A
 *   Output: ['Ralphs', 'KFC', 'Chick-fil-A']
 * @param  {std::string} locations_filename     : locations_filename
 * @return {std::vector<std::string>}           : locations
 */
std::vector<std::string> TrojanMap::ReadLocationsFromCSVFile(
    std::string locations_filename) {
  std::vector<std::string> location_names_from_csv;
  std::fstream fin;
  fin.open(locations_filename, std::ios::in);
  std::string line, word;
  getline(fin, line);
  while (getline(fin, word)) {
    location_names_from_csv.push_back(word);
  }
  fin.close();
  return location_names_from_csv;
}

/**
 * Given CSV filenames, it read and parse dependencise data from CSV file,
 * and return dependencies vector for topological sort problem.
 * We have provided the code for you. Please do not need to change this function.
 * Example:
 *   Input: "topologicalsort_dependencies.csv"
 *   File content:
 *     Source,Destination
 *     Ralphs,Chick-fil-A
 *     Ralphs,KFC
 *     Chick-fil-A,KFC
 *   Output: [['Ralphs', 'Chick-fil-A'], ['Ralphs', 'KFC'], ['Chick-fil-A', 'KFC']]
 * @param  {std::string} dependencies_filename     : dependencies_filename
 * @return {std::vector<std::vector<std::string>>} : dependencies
 */
std::vector<std::vector<std::string>> TrojanMap::ReadDependenciesFromCSVFile(
    std::string dependencies_filename) {
  std::vector<std::vector<std::string>> dependencies_from_csv;
  std::fstream fin;
  fin.open(dependencies_filename, std::ios::in);
  std::string line, word;
  getline(fin, line);
  while (getline(fin, line)) {
    std::stringstream s(line);
    std::vector<std::string> dependency;
    while (getline(s, word, ',')) {
      dependency.push_back(word);
    }
    dependencies_from_csv.push_back(dependency);
  }
  fin.close();
  return dependencies_from_csv;
}

/**
 * DeliveringTrojan: Given a vector of location names, it should return a
 * sorting of nodes that satisfies the given dependencies. If there is no way to
 * do it, return a empty vector.
 *
 * @param  {std::vector<std::string>} locations                     : locations
 * @param  {std::vector<std::vector<std::string>>} dependencies     : prerequisites
 * @return {std::vector<std::string>} results                       : results
 */
std::vector<std::string> TrojanMap::DeliveringTrojan(   std::vector<std::string> &locations,
                                                        std::vector<std::vector<std::string>> &dependencies) {
    std::vector<std::string> result;
    std::map<std::string, int> priorityList;

    // initialize list, each location initially get priority of 0
    for(auto& loc : locations)
        priorityList[loc] = 0;

    // for each dependency pair (location1, location2), the location1 should be visited prior to locaiton2
    for(auto& depPair : dependencies){
        std::string& first = depPair[0];
        priorityList[first] ++;
    }

    // sort the node
    std::vector<std::pair<std::string, int>> tmp(priorityList.begin(), priorityList.end());
    auto sortAlgorithm = [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        return a.second > b.second;
    };

    std::sort(tmp.begin(), tmp.end(), sortAlgorithm);

    for(auto& loc : tmp)
        result.push_back(loc.first);
    return result;
}

/**
 * inSquare: Give a id return whether it is in square or not.
 *
 * @param  {std::string} id            : location id
 * @param  {std::vector<double>} square: four vertexes of the square area
 * @return {bool}                      : in square or not
 */
bool TrojanMap::inSquare(std::string id, std::vector<double> &square) {
    // square = {longitude_left, longitude_right, latitude_upper, latitude_lower}
    bool res;
    double lat = GetLat(id);
    double lon = GetLon(id);
    res = (lon >= square[0] && lon < square[1]) && (lat >= square[3] && lat <= square[2]);
    return res;
}


/**
 * GetSubgraph: Give four vertexes of the square area, return a list of location
 * ids in the squares
 *
 * @param  {std::vector<double>} square         : four vertexes of the square
 * area
 * @return {std::vector<std::string>} subgraph  : list of location ids in the
 * square
 */
std::vector<std::string> TrojanMap::GetSubgraph(std::vector<double> &square) {
    // include all the nodes in subgraph
    std::vector<std::string> subgraph;
    for(auto& pair : data)
    {
        const std::string& id = pair.first;
        if(inSquare(id, square))
            subgraph.push_back(id);
    }
    return subgraph;
}

/**
 * Cycle Detection: Given four points of the square-shape subgraph, return true
 * if there is a cycle path inside the square, false otherwise.
 *
 * @param {std::vector<std::string>} subgraph: list of location ids in the
 * square
 * @param {std::vector<double>} square: four vertexes of the square area
 * @return {bool}: whether there is a cycle or not
 */
bool TrojanMap::CycleDetection(std::vector<std::string> &subgraph, std::vector<double> &square) {

    bool res;
    std::unordered_set<std::string> visited;
    std::stack<std::string> cyclePathToReport;
    res = dfsCycle(subgraph[0], subgraph[0], data, subgraph, visited, cyclePathToReport);

    return res;
}

/**
 * FindNearby: Given a class name C, a location name L and a number r,
 * find all locations in class C on the map near L with the range of r and
 * return a vector of string ids
 *
 * @param {std::string} className: the name of the class
 * @param {std::string} locationName: the name of the location
 * @param {double} r: search radius
 * @param {int} k: search numbers
 * @return {std::vector<std::string>}: location name that meets the requirements
 */
std::vector<std::string> TrojanMap::FindNearby(std::string attributesName, std::string name, double r, int k) {
  std::vector<std::string> res;
  return res;
}

/**
 * Shortest Path to Visit All Nodes: Given a list of locations, return the shortest
 * path which visit all the places and no need to go back to the start point.
 *
 * @param  {std::vector<std::string>} input : a list of locations needs to visit
 * @return {std::vector<std::string> }      : the shortest path
 */
std::vector<std::string> TrojanMap::TrojanPath(
      std::vector<std::string> &location_names) {
    std::vector<std::string> res;
    return res;
}

/**
 * Given a vector of queries, find whether there is a path between the two locations with the constraint of the gas tank.
 *
 * @param  {std::vector<std::pair<double, std::vector<std::string>>>} Q : a list of queries
 * @return {std::vector<bool> }      : existence of the path
 */
std::vector<bool> TrojanMap::Queries(const std::vector<std::pair<double, std::vector<std::string>>>& q) {
    std::vector<bool> ans(q.size());
    return ans;
}

/**
 * CreateGraphFromCSVFile: Read the map data from the csv file
 * We have provided the code for you. Please do not need to change this function.
 */
void TrojanMap::CreateGraphFromCSVFile() {
  // Do not change this function
  std::fstream fin;
  fin.open("src/lib/data.csv", std::ios::in);
  std::string line, word;

  getline(fin, line);
  while (getline(fin, line)) {
    std::stringstream s(line);

    Node n;
    int count = 0;
    while (getline(s, word, ',')) {
      word.erase(std::remove(word.begin(), word.end(), '\''), word.end());
      word.erase(std::remove(word.begin(), word.end(), '"'), word.end());
      word.erase(std::remove(word.begin(), word.end(), '{'), word.end());
      word.erase(std::remove(word.begin(), word.end(), '}'), word.end());
      if (count == 0)
        n.id = word;
      else if (count == 1)
        n.lat = stod(word);
      else if (count == 2)
        n.lon = stod(word);
      else if (count == 3)
        n.name = word;
      else {
        word.erase(std::remove(word.begin(), word.end(), ' '), word.end());
        if (isalpha(word[0])) n.attributes.insert(word);
        if (isdigit(word[0])) n.neighbors.push_back(word);
      }
      count++;
    }
    data[n.id] = n;
  }
  fin.close();
}
