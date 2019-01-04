#ifndef __GAMERUN_H
#define __GAMERUN_H
/*--------------------------------------------------------------------------------
								  Auxiliary Structures
--------------------------------------------------------------------------------*/

#include "../Part1/Headers.hpp"
#include "Thread.hpp"
#include "../Part1/PCQueue.hpp"
#include "../Part1/Semaphore.hpp"
#include "utils.hpp"

#define DEAD_CELL 0
#define LIVE_CELL 1

struct game_params {
	// All here are derived from ARGV, the program's input parameters. 
	uint n_gen;
	uint n_thread;
	string filename;
	bool interactive_on;
	bool print_on;
};

///----------------hw3----------------///
class Task{
public:
	int row_start;
	int row_end;
	Task(int row_start, int row_end): row_start(row_start), row_end(row_end){}
	~Task(){}
};
///----------------hw3----------------///

/*--------------------------------------------------------------------------------
									Class Declaration
--------------------------------------------------------------------------------*/
class Game {
public:

	Game(game_params);
	~Game();
	void run(); // Runs the game
	const vector<float> gen_hist() const; // Returns the generation timing histogram
	const vector<float> tile_hist() const; // Returns the tile timing histogram
	uint thread_num() const; //Returns the effective number of running threads = min(thread_num, field_height)


protected: // All members here are protected, instead of private for testing purposes

	// See Game.cpp for details on these three functions
	void _init_game();
	void _step(uint curr_gen);
	void _destroy_game();

	uint m_gen_num; 			 // The number of generations to run
	uint m_thread_num; 			 // Effective number of threads = min(thread_num, field_height)
	vector<float> m_tile_hist; 	 // Shared Timing history for tiles: First m_gen_num cells are the calculation durations for tiles in generation 1 and so on.
	// Note: In your implementation, all m_thread_num threads must write to this structure.
	vector<float> m_gen_hist;  	 // Timing history for generations: x=m_gen_hist[t] iff generation t was calculated in x microseconds
	vector<Thread*> m_threadpool; // A storage container for your threads.
	// This acts as the threadpool.

	bool interactive_on; // Controls interactive mode - that means, prints the board as an animation instead of a simple dump to STDOUT
	bool print_on; // Allows the printing of the board. Turn this off when you are checking performance (Dry 3, last question)

///----------------hw3----------------///

	PCQueue<Task*> queue;
	bool_mat current_board;   //field
	int board_width;
	int board_height;
	bool_mat next_move_board; //next move field
	Semaphore barrier;    //Semaphore(0)
	Semaphore mutex;  //Semaphore(1)
	int done_tasks_num; //counter for done tasks each generation
	std::string game_name;

	inline void print_board(const char* header);

	class Consumer: public Thread{ //a subclass of the thread class, need to
	public:
		// implement thread_workload (to override the implementation of thread)
		Game* game;
		Consumer(uint thread_id, Game* game): Thread::Thread(thread_id), game(game){}
		~Consumer(){}

	protected:
		void thread_workload() override{
			int neighbors_alive = 0;
			cout << game->board_width << endl;
			while(1){
				Task* t = game->queue.pop();
				if(t->row_start == -1){
					delete(t);
					return;
				}
				auto start_time = std::chrono::system_clock::now();
				for(int i = t->row_start; i < t->row_end; i++){
					for(uint j = 1; j < game->board_width-1; j++){
						//neighbors_alive = 0;
                                                //cout << "i: " << i << " j: " << j << endl;
						// if(game->current_board[i+1][j+1] == 1 ||
						//    game->current_board[i+1][j] == 1 ||
						//    game->current_board[i+1][j-1] == 1 ||
						//    game->current_board[i][j-1] == 1 ||
						//    game->current_board[i][j+1] == 1 ||
						//    game->current_board[i-1][j+1] == 1 ||
						//    game->current_board[i-1][j] == 1 ||
						//    game->current_board[i-1][j-1] == 1 ){
						// 	neighbors_alive++;
						// }
						neighbors_alive = (game->current_board[i+1][j+1] == LIVE_CELL) +
										(game->current_board[i+1][j] == LIVE_CELL) +
										(game->current_board[i+1][j-1] == LIVE_CELL) +
										(game->current_board[i][j-1] == LIVE_CELL) +
										(game->current_board[i][j+1] == LIVE_CELL) +
										(game->current_board[i-1][j+1] == LIVE_CELL) +
										(game->current_board[i-1][j] == LIVE_CELL) +
										(game->current_board[i-1][j-1] == LIVE_CELL);
						//a live cell with different then 2 or number of
						// neighbors will beacome dead cell in the next move
						if(game->current_board[i][j] == LIVE_CELL &&
						   (neighbors_alive != 2 && neighbors_alive != 3)){
							game->next_move_board[i][j] = DEAD_CELL;
						}
						//a dead cell with exactly 3 neighbors will beacome
						// alive in the next move
						if(game->current_board[i][j] == DEAD_CELL &&
						   neighbors_alive == 3){
							game->next_move_board[i][j] = LIVE_CELL;
						}
					}
				}
				delete(t);
				this->game->mutex.down();
				game->done_tasks_num++;
				if(game->done_tasks_num == game->m_thread_num)
					this->game->barrier.up();
				auto end_time = std::chrono::system_clock::now();
				(game->m_tile_hist).push_back((float) std::chrono::duration_cast
						<std::chrono::microseconds>(end_time - start_time).count());
				this->game->mutex.up();
			}
		}


	};

};



///----------------hw3----------------///

#endif
