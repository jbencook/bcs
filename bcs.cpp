#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

class Games {
public:
  vector<string> hTeam;
  vector<string> vTeam;
  vector<int> vScore;
  vector<int> hScore;
};

Games readGames();
Games games = readGames();

vector<string> readRankings();
vector<string> rankings = readRankings();
int getRank(string team);

class Graph {
public:
  set<string> teamSet;
  vector<string> teamVec;
  vector<set<int> > edges;
  vector<set<int> > visitList;
};

Graph loadGraph();
Graph graph = loadGraph();
void resetVisited(bool * vis, int n);
void explore(bool * vis, int v);


int main() {

  ofstream output;
  output.open("output/results.txt");
  //Get Type I Inconsistencies:
  set<vector<string> > incon1;
  vector<string> tempIncon1;

  for(int i = 0; i < games.hTeam.size(); i++) {
    if(games.hScore.at(i) > games.vScore.at(i)) {//home team wins:
      if(getRank(games.hTeam.at(i)) > getRank(games.vTeam.at(i))) {
	tempIncon1.push_back(games.hTeam.at(i));
	tempIncon1.push_back(games.vTeam.at(i));
	incon1.insert(tempIncon1);
	tempIncon1.clear();
      }
    }
    else {//away team wins:
      if(getRank(games.hTeam.at(i)) < getRank(games.vTeam.at(i))) {
	tempIncon1.push_back(games.vTeam.at(i));
	tempIncon1.push_back(games.hTeam.at(i));
	incon1.insert(tempIncon1);
	tempIncon1.clear();
      }
    }
  }
  
  output << endl;
  output << "Type I Inconsistencies: " << endl;
  output << endl;
  set<vector<string> >::iterator it1;
  int count = 0;
  for(it1 = incon1.begin(); it1 != incon1.end(); it1++) {
    count++;
    vector<string> temp = *it1;
    output << count << ": " << temp.at(0) << "("; 
    if(getRank(temp.at(0)) == 26)
      output << "NR";
    else
      output <<getRank(temp.at(0));
    output << ") beat " << temp.at(1) << "(";
    if(getRank(temp.at(1)) == 26)
      output << "NR";
    else
      output << getRank(temp.at(1));
    output << ")" << endl;
  }
  
  //Get Type II Inconsistencies:
  set<set<int> > incon2;
  set<int> tempIncon2;

  for(int i = 0; i < graph.teamSet.size()-1; i++) {
    for(int j = i+1; j < graph.teamSet.size(); j++) {
      if(graph.visitList.at(i).count(j) == 1 && graph.visitList.at(j).count(i) == 1) {
	tempIncon2.insert(i);
	tempIncon2.insert(j);
	incon2.insert(tempIncon2);
	tempIncon2.clear();
      }
    }
  }

  output << endl;
  output << "Type I Inconsistencies: " << endl;
  output << endl;
  set<set<int> >::iterator it2;
  count = 0;
  for(it2 = incon2.begin(); it2 != incon2.end(); it2++) {
    count++;
    tempIncon2 = *it2;
    output << count << ": ";
    set<int>::iterator it3;
    for(it3 = tempIncon2.begin(); it3 != tempIncon2.end(); it3++) {
      if(it3 == tempIncon2.begin()) {
	output << graph.teamVec.at(*it3) << "(";
	if(getRank(graph.teamVec.at(*it3)) == 26)
	  output << "NR";
	else
	  output << getRank(graph.teamVec.at(*it3));
	output << ") <-> ";
      }
      else {
	output << graph.teamVec.at(*it3) << "(";
	if(getRank(graph.teamVec.at(*it3)) == 26)
	  output << "NR";
	else
	  output << getRank(graph.teamVec.at(*it3));
	output << ")" << endl;
      }
    }
  }

  return 0;
}

Games readGames() {
  Games data;

  ifstream gameFile;
  istringstream ss;
  string value;
  int row, col;

  gameFile.open("games.csv");
  row = 0;
  while(gameFile.good()) {
      getline(gameFile, value);
      istringstream ss(value);
      string token;
      if(row > 0){
	col = 0;
	while(getline(ss, token, ',')) {
	  if(col == 1) {
	    data.vTeam.push_back(token);
	  }
	  if(col == 2) {
	    int tempInt;
	    istringstream ss(token);
	    ss >> tempInt;
	    data.vScore.push_back(tempInt);
	  }
	  if(col == 3) {
	    data.hTeam.push_back(token);
	  }
	  if(col == 4) {
	    int tempInt;
	    istringstream ss(token);
	    ss >> tempInt;
	    data.hScore.push_back(tempInt);
	  }
	  col++;
	}
      }
      row++;
  }
  gameFile.close();

  return data;
}

vector<string> readRankings() {
  vector<string> rank;

  ifstream rankFile;
  string value;
  int col;

  rankFile.open("rankings.csv");
  while(rankFile.good()) {
    getline(rankFile, value);
    istringstream ss(value);
    string token;
    col = 0;
    while(getline(ss, token, ',')) {
      if(col == 1) {
	token.erase(token.end() - 1);
	rank.push_back(token);
      }
      col++;
    }
  }
  rankFile.close();

  rank.insert(rank.begin(), "");

  return rank;
}

int getRank(string team) {
  
  for(int i = 1; i < 26; i++) {
    if(team.compare(rankings.at(i)) == 0) {
      return i;
    }
  }

  return 26;
}

Graph loadGraph() {
  Graph data;

  //load teamSet:
  for(int i = 0; i < games.hTeam.size(); i++) {
    data.teamSet.insert(games.hTeam.at(i));
    data.teamSet.insert(games.vTeam.at(i));
  }
  
  //load teamVec:
  set<string>::iterator it;
  for(it = data.teamSet.begin(); it != data.teamSet.end(); it++) {
    data.teamVec.push_back(*it);
  }

  //load edge list:
  set<int> temp;
  data.edges.insert(data.edges.begin(), data.teamSet.size(), temp);

  for(int i = 0; i < games.hTeam.size(); i++) {
    if(games.hScore.at(i) > games.vScore.at(i)) {
      set<string>::iterator winner = data.teamSet.find(games.hTeam.at(i));
      set<string>::iterator loser = data.teamSet.find(games.vTeam.at(i));
      data.edges.at(distance(data.teamSet.begin(), winner)).insert(distance(data.teamSet.begin(), loser));
    }
    else {
      set<string>::iterator winner = data.teamSet.find(games.vTeam.at(i));
      set<string>::iterator loser = data.teamSet.find(games.hTeam.at(i));
      data.edges.at(distance(data.teamSet.begin(), winner)).insert(distance(data.teamSet.begin(), loser));
    }
  }

  //load visit list:
  bool visited[data.teamSet.size()];

  resetVisited(visited, data.teamSet.size());
  for(int i = 0; i < data.teamSet.size(); i++) {
    explore(visited, i);
    for(int j = 0; j < data.teamSet.size(); j++) {
      if(i != j && visited[j]) {
	temp.insert(j);
      }
    }
    data.visitList.push_back(temp);
    temp.clear();
    resetVisited(visited, data.teamSet.size());
  }

  return data;
}

void resetVisited(bool * vis, int n) {
  for(int i = 0; i < n; i++)
    vis[i] = false;
}

void explore(bool * vis, int v) {//is there a path from a to b?
  vis[v] = true;
  set<int>::iterator it;
  for(it = graph.edges.at(v).begin(); it != graph.edges.at(v).end(); it++) {
    if(!vis[*it])
      explore(vis, *it);
  }
}
