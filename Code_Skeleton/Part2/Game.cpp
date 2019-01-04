#include "Game.hpp"

#define DEAD_CELL 0
#define LIVE_CELL 1

#define ZERO '0'
#define SPACE ' '
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
Game::Game(game_params params): m_threadpool(), queue(), barrier(0), mutex(1){
	//TODO create board from file and set sizes and num of threads and num of gen
    this->m_gen_num = params.n_gen;
    this->game_name = params.filename;
    this->m_thread_num = params.n_thread;
    this->interactive_on = params.interactive_on;
    this->print_on = params.print_on;
}

Game::~Game(){
    _destroy_game();
}

void Game::run() {
	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((float)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(NULL);
	} // generation loop
	print_board("Final Board");
	_destroy_game();
}

void Game::_init_game() {
    done_tasks_num = 0;

	// TODO Create game fields
    vector<std::string> lines = utils::read_lines(game_name);
    vector<string> vec = utils::split(lines[0], SPACE);
    this->current_board = bool_mat(lines.size() + 2, vector<bool>(vec.size() + 2));
    this->next_move_board = bool_mat(lines.size() + 2, vector<bool>(vec.size() + 2));

    for(uint i = 0; i < lines.size()+2; i++){
        for(uint j = 0; j < vec.size()+2; j++){
            this->current_board[i][j] = 0;
        }
    }

    vector<vector<string>> temp;
    
    for(uint i = 0; i < lines.size(); i++){
        temp.push_back(utils::split(lines[i],SPACE));
    }
    
    board_height = lines.size();
    board_width = lines[0].size();

    for(uint i = 0; i < temp.size(); i++){
        for(uint j = 0; j < temp[0].size(); j++){
            if(temp[i][j] == "0"){
                current_board[i + 1][j + 1] = DEAD_CELL;
            }
            else{
                current_board[i + 1][j + 1] = LIVE_CELL;
            }
        }
    }
    next_move_board = current_board;


    // Create threads
    this->m_thread_num = thread_num();
    for(uint i = 0; i < m_thread_num; i++){
        this->m_threadpool.push_back(new Consumer(i,this));
    }
    // Start the threads
    for(uint i = 0; i < m_thread_num; i++){
        this->m_threadpool[i]->start();
    }
	// Testing of your implementation will presume all threads are started here
}

void Game::_step(uint curr_gen) {
	// Push jobs to queue
	int size_row = board_height / m_thread_num;
	int curr_start = 1;
	done_tasks_num = 0;
	for(uint i = 0; i < m_thread_num; i++){
        Task* t = new Task(curr_start , curr_start + size_row +
                    (board_height % m_thread_num));
		this->queue.push(t);
		curr_start += size_row;
	}

	// Wait for the workers to finish calculating
	this->barrier.down();

	// Swap pointers between current and next field
	current_board = next_move_board;

	//TODO: something with curr_gen
}

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources
    for(uint i = 0; i < this->m_thread_num; i++){
        Task* t = new Task(-1,2); //so it will be deleted
        queue.push(t);
    }
    //wait till all the threads are finished
    
    //delete all the threads
    
     m_threadpool.erase(m_threadpool.begin(), m_threadpool.end());
    
	// Not implemented in the Game's destructor for testing purposes. 
	// Testing of your implementation will presume all threads are joined here
}

const vector<float> Game::gen_hist() const{
	return m_gen_hist;
}

const vector<float> Game::tile_hist() const{
	return m_tile_hist;
}

uint Game::thread_num() const {
    if((uint)board_height >= m_thread_num){
        return m_thread_num;
    }
    return (uint)board_height;
}



/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/

inline void Game::print_board(const char* header) {

	if(print_on){

		// Clear the screen, to create a running animation 
		if(this->interactive_on)
			system("clear");

		// Print small header if needed
		if (header != NULL)
			cout << "<------------" << header << "------------>" << endl;

		// TODO: Print the board
        uint field_width = (int)current_board[0].size() - 2 ;
        uint field_height = (int)current_board.size() - 2;
        bool_mat field = this->current_board;
        cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
        for (uint i = 1; i <= field_height; ++i) {
            cout << u8"║";
            for (uint j = 1; j <= field_width; ++j) {
                cout << (field[i][j] ? u8"█" : u8"░");
            }
            cout << u8"║" << endl;
        }
        cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}

}



/* Function sketch to use for printing the board. You will need to decide its placement and how exactly 
	to bring in the field's parameters. 

		cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
				cout << (field[i][j] ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/ 



