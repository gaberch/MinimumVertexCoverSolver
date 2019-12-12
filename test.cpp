/**
 * An example unit test using doctest. See documentation at
 * https://github.com/onqtam/doctest/blob/master/doc/markdown/tutorial.md
 */
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"



class AdjacencyMatrix
{

private:

   std::vector <int> *adjacency_matrix = nullptr;
   int vertex_number = 0;

   bool reset_matrix(int num1, int num2){
      if ((num1 == num2) || (num1 >= vertex_number || num2 >= vertex_number))
      {
         delete[] adjacency_matrix;
         adjacency_matrix = new std::vector <int> [vertex_number];
         return false;
      }
      return true;
   }

public:

   void new_matrix(int vertex)
   {
      if (adjacency_matrix != nullptr)
      {
         delete[] adjacency_matrix;
         adjacency_matrix = nullptr;
      }
      vertex_number = vertex;
      adjacency_matrix = new std::vector<int>[vertex_number];
   }

   bool add_matrix_element(int v1, int v2)
   {
      if (reset_matrix(v1,v2))
      {
         adjacency_matrix[v1].push_back(v2);
         adjacency_matrix[v2].push_back(v1);
         return true;
      }
      return false;
   }

   bool check_bounds (int v1, int v2)
   {
      return !(v1 >= vertex_number || v2 >= vertex_number);
   }

   int read_matrix(int r, int c)
   {
      return adjacency_matrix[r][c];
   }

   int matrix_row_size(int r)
   {
      return adjacency_matrix[r].size();
   }

};

class Graph
{

private:


   void print_shortest_path (std::list<int> shortest_path)
   {
      unsigned i = 0;
      for (auto vertex : shortest_path)
      {
         if (i < shortest_path.size() - 1)
         {
            std::cout << vertex << "-";

         }
         else
         {
            std::cout << vertex << std::endl;
         }
         i++;
      }
   }

public:
   int vertex_number = 0;
   AdjacencyMatrix adj_matrix;


   bool check_bounds(int v1, int v2)
   {
      return adj_matrix.check_bounds(v1, v2);
   }

   void new_matrix (int vertex)
   {
      vertex_number = vertex;
      adj_matrix.new_matrix(vertex_number);
   }

   bool add_matrix_element(int v1, int v2)
   {
      return adj_matrix.add_matrix_element(v1,v2);
   }

   bool bfs_search(int init, int final)
   {

      std::list<int> current_queue;
      std::list<int> previous_v;
      std::list<int> next_v;
      std::list<int> shortest_path;

      bool *vertex_checked;
      vertex_checked = new bool[vertex_number];

      for (int i = 0; i < vertex_number; i++)
      {
         vertex_checked[i] = false;
      }
      vertex_checked[init] = true;
      current_queue.push_back(init);

      // Breadth First Search
      while(!current_queue.empty())
      {
         int row = current_queue.front();
         current_queue.pop_front();
         for (int col = 0; col < adj_matrix.matrix_row_size(row); col++)
         {

            if (!vertex_checked[adj_matrix.read_matrix(row,col)])
            {

               next_v.push_back(adj_matrix.read_matrix(row,col));
               previous_v.push_back(row);
               current_queue.push_back(adj_matrix.read_matrix(row,col));
               vertex_checked[adj_matrix.read_matrix(row,col)] = true;
            }

            if (adj_matrix.read_matrix(row,col) == final)
            {
               int last_v = final;
               while(!next_v.empty())
               {
                  if (next_v.back() != last_v)
                  {
                     next_v.pop_back();
                     previous_v.pop_back();
                  }
                  else
                  {
                     shortest_path.push_back(next_v.back());
                     last_v = previous_v.back();
                     previous_v.pop_back();
                     next_v.pop_back();
                  }
               }
               shortest_path.push_back(init);
               shortest_path.reverse();
               print_shortest_path(shortest_path);
               while (!current_queue.empty())
               {
                  current_queue.pop_front();
               }
               delete[] vertex_checked;
               return true;
            }
         }
      }

      delete[] vertex_checked;
      return false;
   }

};

class Input_Parser
{

private:

   int flag = 0;
   Graph graph;
   int n = 0;
   int num_vecs = 2;
   bool found_duplicate_edge = false;
   bool duplicate_clause_entry_v1 = false;
   bool duplicate_clause_entry_v2 = false;




   void vertex_cmd (std::string input, std::size_t whitespace)
   {
      char *end;
      input = input.substr(whitespace, input.length() - 1);
      int num_vertices = strtol(input.c_str(), &end, 10);
      if (num_vertices < 2)
      {
         std::cerr << "Error: Set of vertices need to be at least 2\n";
      }
      else
      {
         graph.new_matrix(num_vertices);
         n = num_vertices;
         flag = 1;
         edge_vector[0].clear();
         edge_vector[1].clear();
      }
   }

   void edge_cmd (const std::string &input, std::size_t whitespace)
   {
      if (flag == 2)
      {
         std::cerr << "Error: Set of edges have already been captured. To enter a new set of edges, a new vertex command must be entered first" << '\n';
      }
      else if (flag == 0)
      {
         std::cerr << "Error: Number of vertices need to be inputted first\n";
      }
      else
      {
         std::string edge_set = input.substr(whitespace, input.length()-1);
         char edge_set_rb = '}';
         std::size_t current_pos;
         std::string vertex1, vertex2;
         current_pos = edge_set.find(edge_set_rb, 0);
         while (current_pos != std::string::npos)
         {
            char *end;
            std::size_t x = edge_set.find(',', 0) - 1 - edge_set.find('<', 0);
            std::size_t y = edge_set.find('>', 0) - 1 - edge_set.find(',', 0);
            vertex1 = edge_set.substr(edge_set.find('<', 0) + 1, x);
            vertex2 = edge_set.substr(edge_set.find(',', 0) + 1, y);
            int v1 = strtol(vertex1.c_str(),&end,10);
            int v2 = strtol(vertex2.c_str(),&end,10);

            if (v1 == v2)
            {
               edge_set.erase(0,edge_set.find_first_of('>', 0) + 2);
               flag = 2;
               current_pos = edge_set.find(edge_set_rb, 0);
               continue;
            }

            if (!(graph.add_matrix_element(v1,v2)))
            {
               std::cerr << "Error: Edge specified is out of bounds\n";
               flag = 0;
               break;
            }
            for (unsigned i = 0; i < edge_vector[0].size(); i++){
               if ((edge_vector[0][i] == v1 && edge_vector[1][i] == v2) || (edge_vector[0][i] == v2 && edge_vector[1][i] == v1)){
                  found_duplicate_edge = true;
               }
            }
            if (!found_duplicate_edge){
               edge_vector[0].push_back(v1);
               edge_vector[1].push_back(v2);
            }
            found_duplicate_edge = false;
            edge_set.erase(0,edge_set.find_first_of('>', 0) + 2);
            flag = 2;
            current_pos = edge_set.find(edge_set_rb, 0);
         }

      }
   }

   void create_cnf(){

      if (flag == 2){


         std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());

         Minisat::vec<Minisat::Lit> literal_set;
         Minisat::vec<Minisat::Lit> clause1;
         // Minisat::vec<Minisat::Lit> clause2;
         // Minisat::vec<Minisat::Lit> clause3;
         Minisat::vec<Minisat::Lit> clause4;

         for (int k = 1; k <= n; k++){
            // auto start = std::chrono::system_clock::now();
            literal_set.clear();

            //initialize literal_set

            for (int i = 0; i < n; i++){
               for (int j = 0; j < k; j++){
                  Minisat::Lit literal;
                  literal = Minisat::mkLit(solver->newVar());
                  literal_set.push(literal);
               }
            }

            //clause 1
            // for all i element of [1,k], a clause (x[1,i], x[2,i], ... , x[n,i])
            // std::cout << "Clause 1 ------------------------------" << std::endl;
            // std::string should_clause1 = "(";
            // std::string actual_clause1 = "(";
            for (int i = 1; i <= k; i ++){
               clause1.clear();
               for (int r = 1; r <= n; r++){
                  clause1.push(literal_set[(r - 1)*k + (i - 1) ]);
                  // should_clause1 += ", x[" + std::to_string(r) + "," + std::to_string(i) + "]";
                  // actual_clause1 += ", x[" + std::to_string((r - 1)*k + (i - 1)) +"]";
                  // std::cout << "n = " << n << ", k = " << k << ", r = " << r << ", i = " << i << std::endl;
               }
               // std::cout << "Should add to clause 1 - " << should_clause1 << std::endl;
               // std::cout << "Adding to clause 1 - " << actual_clause1 << std::endl;
               solver->addClause(clause1);
            }
            //clause 2
            // For all m element of [1,n], for all p,q element of [1,k] with p < q
            // a clause (~x[m,p], ~x[m,q])
            // std::cout << "Clause 2 ------------------------------" << std::endl;
            for (int q = 1; q <= k; q++){
               for (int p = 1; p < q; p++){
                  for (int m = 1; m <= n; m++){
                     solver->addClause(~literal_set[(m - 1)*k + (p - 1)],~literal_set[(m - 1)*k + (q - 1)]);

                     // std::cout << "n = " << n << ", k = " << k << ", q = " << q << ", p = " << p << ", m = " << m << std::endl;
                     // std::cout << "Should add clause (~x[" << m << "," << p <<"], ~x[" << m << "," << q << "])" << std::endl;
                     // std::cout << "Adding clause (~x[" << (m - 1)*k + (p - 1) << "], ~x[" << (m - 1)*k + (q - 1) << "])" << std::endl;
                     // std::cout << std::endl;
                  }
               }
            }

            //clause 3
            // For all m element of [1,k], for all p,q element of [1,n] with p < q
            // a clause (~x[p,m], ~x[q,m])
            // std::cout << "Clause 3 ------------------------------" << std::endl;
            for (int m = 1; m <= k; m++){
               for (int q = 1; q <= n; q++){
                  for (int p = 1; p < q; p++){
                     solver->addClause(~literal_set[(p - 1)*k + (m - 1)],~literal_set[(q - 1)*k + (m - 1)]);
                     // std::cout << "n = " << n << ", k = " << k << ", q = " << q << ", p = " << p << ", m = " << m << std::endl;
                     // std::cout << "Should add clause (~x[" << p << "," << m <<"], ~x[" << q << "," << m << "])" << std::endl;
                     // std::cout << "Adding clause (~x[" << (p - 1)*k + (m - 1) << "], ~x[" << (q - 1)*k + (m - 1) << "])" << std::endl;
                     // std::cout << std::endl;
                  }
               }
            }



            //clause 4
            // For all <i,j> element of Edge Set, a clause (x[i,1], x[i,2], ... , x[i,k], x[j,1], x[j,2], ... , x[j,k])
            // std::cout << "Clause 4 ------------------------------" << std::endl;
            for (unsigned i = 0; i < edge_vector[0].size(); i++){
               clause4.clear();
               // std::string should_clause = "(";
               // std::string actual_clause = "(";
               std::vector <int> clause_entry;
               for (int c = 1; c <= k; c++){
                  // std::cout << "n = " << n << ", k = " << k << ", i = " << i << ", c = " << c << ", edge = <" << edge_vector[0][i] << "," << edge_vector[1][i] << ">" << std::endl;
                  //look for duplicate entries
                  for (auto entry: clause_entry){
                     if (entry == edge_vector[0][i]*k + (c - 1)){
                        duplicate_clause_entry_v1 = true;
                     }
                     if (entry == edge_vector[1][i]*k + (c - 1)){
                        duplicate_clause_entry_v2 = true;
                     }
                  }
                  if (!duplicate_clause_entry_v1){
                     clause4.push(literal_set[edge_vector[0][i]*k + (c - 1)]);
                     // std::cout << "Should be adding to clause (x[" << edge_vector[0][i] + 1 << "," << c <<"])" << std::endl;
                     // std::cout << "Adding to clause (x[" << edge_vector[0][i]*k + (c - 1) << "])" << std::endl;
                     // should_clause += ", x[" + std::to_string(edge_vector[0][i] + 1) + "," + std::to_string(c) + "]";
                     // actual_clause += ", x[" + std::to_string( edge_vector[0][i]*k + (c - 1)) + "]";
                  }
                  if (!duplicate_clause_entry_v2){
                     clause4.push(literal_set[edge_vector[1][i]*k + (c - 1)]);
                     // std::cout << "Should be adding to clause (x[" << edge_vector[1][i] + 1 << "," << c << "])" << std::endl;
                     // should_clause += ", x[" + std::to_string(edge_vector[1][i] + 1) + "," + std::to_string(c) + "]";
                     // std::cout << "Adding to clause (x[" << edge_vector[1][i]*k + (c - 1) << "])" << std::endl;
                     // actual_clause += ", x["  +std::to_string( edge_vector[1][i]*k + (c - 1)) + "]";
                  }
                  duplicate_clause_entry_v1 = false;
                  duplicate_clause_entry_v2 = false;

                  // std::cout << "Should be adding to clause (x[" << edge_vector[0][i] + 1 << "," << c <<"], x[" << edge_vector[1][i] + 1 << "," << c << "])" << std::endl;
                  // should_clause += ", x[" + std::to_string(edge_vector[0][i] + 1) + "," + std::to_string(c) + "], x[" + std::to_string(edge_vector[1][i] + 1) + "," + std::to_string(c) + "]";
                  // std::cout << "Adding to clause (x[" << edge_vector[0][i]*k + (c - 1) << "], x[" << edge_vector[1][i]*k + (c - 1) << "])" << std::endl;
                  // actual_clause += ", x[" + std::to_string( edge_vector[0][i]*k + (c - 1)) + "], x[" +std::to_string( edge_vector[1][i]*k + (c - 1)) + "]";
                  // std::cout << std::endl;
               }

               // should_clause += ")";
               // actual_clause += ")";
               // std::cout<< std::endl;
               // std::cout << "Clause should be " << should_clause << std::endl;
               // std::cout << "Adding clause " << actual_clause << std::endl;

               solver->addClause(clause4);
               // std::cout<< std::endl;

            }

            // if (k == 1){
            //    for (int i = 0; i < edge_vector[0].size(); i++){
            //       for (int j = 0; j < 2; j++){
            //          std::cout << edge_vector[j][i] << " ";
            //       }
            //       std::cout << std::endl;
            //    }
            // }

            bool res = solver->solve();
            if (res){
               // std::cout << "Satisfying assignment found for k = " << k << std::endl;
               std::vector <int> vertex_cover_list;

                  for (int r = 0; r < n; r++) {
                       for (int c = 0; c < k; c++) {
                           if (solver->modelValue(literal_set[r*k + c]) == Minisat::l_True) {
                               vertex_cover_list.push_back(r);
                           }
                       }
                   }

                   std::sort(vertex_cover_list.begin(), vertex_cover_list.end());
                   for (unsigned x = 0; x < vertex_cover_list.size(); x++){
                     if (x + 1 != vertex_cover_list.size()){
                        std::cout << vertex_cover_list[x] << " ";
                     }
                     else{
                        std::cout << vertex_cover_list[x] << std::endl;
                     }

                   }

               break;
            }
            // else{
               // std::cout << "No solution found for k = " << k << std::endl;
            // }
            // auto end = std::chrono::system_clock::now();
            // std::chrono::duration<double> diff = end-start;
            // std::clog << "Duration = " << diff.count() << " seconds" << std::endl << std::endl;
            solver.reset (new Minisat::Solver());
         }

         // std::cout << "triple for loop is complete\n";
      }
      else{
         std::cerr << "Error: Not enough information provided for graph input\n";
      }

   }

   void path_cmd(std::string input, size_t whitespace)
   {
      if (flag == 2)
      {
         input = input.substr(whitespace + 1, input.length() - 1);
         std::size_t whitespace = input.find(' ', 0);
         std::string v1 = input.substr(0,whitespace);
         std::string v2 = input.substr(whitespace + 1, input.length() - 1);
         if (!v1.empty() && !v2.empty())
         {
            char *end;
            int init = strtol(v1.c_str(),&end,10);
            int final = strtol(v2.c_str(),&end,10);
            if (!graph.check_bounds(init,final))
            {
               std::cerr << "Error: Vertices specified are not in graph specified\n";
            }
            else if (init == final)
            {
               std::cout << init << "-" << final << std::endl;
            }
            else if (!graph.bfs_search(init,final))
            {
               std::cerr << "Error: Path does not exist\n";
            }
         }
      }
      else
      {
         std::cerr << "Error: Not enough information provided for graph input\n";
      }
   }

public:
   std::vector<std::vector<int>> edge_vector;


   void cmd_parser(const std::string &input)
   {
      int v_input = input.find('V',0);
      int e_input = input.find('E', 0);
      int s_input = input.find('s', 0);
      int whitespace = input.find(' ', 0);

      if ((v_input != -1) && (whitespace != -1))
      {
         vertex_cmd(input, whitespace);
      }
      else if ((e_input != -1) && (whitespace != -1))
      {
         edge_cmd(input, whitespace);
         create_cnf();
      }
      else if ((s_input != -1) && (whitespace != -1))
      {
         path_cmd(input, whitespace);
      }
      else if (!input.empty())
      {
         std::cerr << "Error: Invalid command\n";
      }

   }

};




TEST_CASE("Test 1") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 3";
   Parser.cmd_parser(input);
   input = "E {<0,1>,<0,2>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0\n");
}

TEST_CASE("Test 2") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 3";
   Parser.cmd_parser(input);
   input = "E {<0,1>,<1,2>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "1\n");
}

TEST_CASE("Test 3") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 5";
   Parser.cmd_parser(input);
   input = "E {<0,3>,<0,4>,<4,1>,<3,2>,<3,1>,<3,4>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "3 4\n");
}

TEST_CASE("Test 4") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 3";
   Parser.cmd_parser(input);
   input = "E {<0,2>,<1,2>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "2\n");
}

TEST_CASE("Test 5") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 4";
   Parser.cmd_parser(input);
   input = "E {<2,3>,<0,1>,<2,0>,<1,3>,<2,1>,<3,0>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 1 2\n");
}

TEST_CASE("Test 6") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 4";
   Parser.cmd_parser(input);
   input = "E {<0,1>,<2,0>,<1,3>,<3,0>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 1\n");
}

TEST_CASE("Test 7") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 6";
   Parser.cmd_parser(input);
   input = "E {<0,1>,<1,2>,<2,3>,<3,4>,<4,5>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 2 4\n");
}

TEST_CASE("Test 8") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 6";
   Parser.cmd_parser(input);
   input = "E {<0,1>,<1,2>,<2,3>,<3,4>,<4,5>,<1,5>,<1,3>,<3,5>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "1 3 4\n");
}

TEST_CASE("Test 9") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 6";
   Parser.cmd_parser(input);
   input = "E {<0,1>,<1,2>,<2,3>,<3,4>,<4,5>,<1,5>,<1,3>,<3,5>,<0,2>,<0,5>,<2,5>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "1 2 4 5\n");
}

TEST_CASE("Test 10") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 10";
   Parser.cmd_parser(input);
   input = "E {<3,8>,<4,6>,<2,5>,<2,3>,<5,9>,<0,3>,<1,7>,<9,1>,<5,3>,<3,7>,<1,5>,<4,5>,<2,1>,<0,1>,<1,8>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "1 3 4 5\n");
}

TEST_CASE("Test 11") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 10";
   Parser.cmd_parser(input);
   input = "E {<4,0>,<6,8>,<7,5>,<3,4>,<6,1>,<1,5>,<2,6>,<7,4>,<0,8>,<0,5>,<3,5>,<0,9>,<4,2>,<0,1>,<8,2>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 2 4 5 6\n");
}

TEST_CASE("Test 12") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 15";
   Parser.cmd_parser(input);
   input = "E {<8,13>,<5,10>,<0,4>,<2,1>,<10,2>,<13,7>,<0,13>,<4,12>,<1,12>,<8,6>,<7,11>,<9,1>,<1,13>,<0,9>,<14,8>,<5,6>,<0,12>,<10,3>,<11,6>,<4,10>,<0,10>,<1,6>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 1 4 6 7 8 10\n");
}

TEST_CASE("Test 13") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 12";
   Parser.cmd_parser(input);
   input = "E {<0,4>,<3,10>,<0,5>,<6,2>,<3,5>,<2,1>,<0,9>,<6,5>,<3,9>,<7,4>,<0,6>,<0,3>,<2,7>,<4,3>,<1,4>,<3,2>,<4,5>,<9,1>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 1 2 3 4 5\n");
}

TEST_CASE("Test 14") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 11";
   Parser.cmd_parser(input);
   input = "E {<2,0>,<4,5>,<3,4>,<10,3>,<4,0>,<1,7>,<9,0>,<6,0>,<9,2>,<4,1>,<4,6>,<9,5>,<3,9>,<5,0>,<1,0>,<0,10>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 3 4 7 9\n");
}

TEST_CASE("Test 15") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 10";
   Parser.cmd_parser(input);
   input = "E {<8,0>,<5,3>,<4,1>,<6,5>,<3,0>,<9,3>,<2,5>,<3,1>,<4,2>,<5,0>,<5,1>,<1,0>,<7,1>,<4,5>,<1,6>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 1 2 3 5\n");
}

TEST_CASE("Test 16") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 9";
   Parser.cmd_parser(input);
   input = "E {<8,0>,<2,5>,<7,3>,<5,8>,<8,1>,<5,0>,<4,7>,<1,0>,<4,6>,<0,6>,<4,1>,<2,4>,<2,3>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 1 3 4 5\n");
}

TEST_CASE("Test 17") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 9";
   Parser.cmd_parser(input);
   input = "E {<1,7>,<0,8>,<7,3>,<4,3>,<6,1>,<1,8>,<4,7>,<8,2>,<5,4>,<2,3>,<1,0>,<5,1>,<6,5>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "1 3 4 5 8\n");
}

TEST_CASE("Test 18") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 7";
   Parser.cmd_parser(input);
   input = "E {<3,1>,<1,6>,<4,2>,<1,2>,<5,3>,<5,1>,<6,0>,<1,4>,<0,5>,<2,0>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 1 2 3\n");
}

TEST_CASE("Test 19") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 6";
   Parser.cmd_parser(input);
   input = "E {<5,1>,<4,1>,<0,3>,<2,1>,<0,1>,<2,0>,<2,3>,<2,4>,<3,1>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 1 2\n");
}

TEST_CASE("Test 20") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 5";
   Parser.cmd_parser(input);
   input = "E {<1,2>,<4,1>,<3,1>,<0,4>,<2,4>,<3,0>,<2,3>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 1 2\n");
}

TEST_CASE("Test 21") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 5";
   Parser.cmd_parser(input);
   input = "E {<1,0>,<1,4>,<0,3>,<3,1>,<1,2>,<0,2>,<4,0>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 1\n");
}

TEST_CASE("Test 22") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 14";
   Parser.cmd_parser(input);
   input = "E {<9,0>,<8,2>,<0,7>,<3,10>,<8,9>,<0,6>,<3,5>,<1,5>,<10,8>,<10,9>,<0,12>,<10,1>,<8,11>,<6,12>,<5,11>,<0,3>,<4,8>,<12,2>,<3,9>,<6,5>,<12,5>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 3 5 8 10 12\n");
}

TEST_CASE("Test 23") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 14";
   Parser.cmd_parser(input);
   input = "E {<2,0>,<12,0>,<7,9>,<4,8>,<13,3>,<8,11>,<0,10>,<11,4>,<6,2>,<5,9>,<8,7>,<4,1>,<6,5>,<4,10>,<11,3>,<1,6>,<3,4>,<1,11>,<2,7>,<0,5>,<7,4>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 4 5 6 7 11 13\n");
}

TEST_CASE("Test 24") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 14";
   Parser.cmd_parser(input);
   input = "E {<2,0>,<12,0>,<7,9>,<4,8>,<13,3>,<8,11>,<0,10>,<11,4>,<6,2>,<5,9>,<8,7>,<4,1>,<6,5>,<4,10>,<11,3>,<1,6>,<3,4>,<1,11>,<2,7>,<0,5>,<7,4>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 4 5 6 7 11 13\n");
}

TEST_CASE("Test 25") {
   std::ostringstream oss;
   std::streambuf* p_cout_streambuf = std::cout.rdbuf();
   std::cout.rdbuf(oss.rdbuf());
   Input_Parser Parser;
   Parser.edge_vector.push_back(std::vector<int>(0));
   Parser.edge_vector.push_back(std::vector<int>(0));

   std::string input = "V 16";
   Parser.cmd_parser(input);
   input = "E {<13,10>,<4,5>,<13,8>,<3,12>,<11,5>,<4,9>,<10,8>,<6,8>,<7,0>,<9,3>,<2,12>,<4,1>,<8,0>,<10,1>,<7,3>,<6,14>,<1,5>,<8,12>,<4,0>,<11,4>,<3,11>,<15,8>,<8,14>,<2,10>}";
   Parser.cmd_parser(input);
   std::cout.rdbuf(p_cout_streambuf);

   CHECK(oss.str() == "0 2 3 4 5 6 8 10\n");
}

// TEST_CASE("Failing Test Examples") {
//     CHECK(true == false);
// }
