#include "gol.h"
#include <pthread.h>
#include <iostream>
#include <cstdlib>

struct Barrier 
{
    pthread_mutex_t mutex;   
    pthread_cond_t  cond;   
    int count;               
    int num_threads;         
    int generation;          
};

static void barrier_init(Barrier *barrier, int num_threads) 
{
    barrier->count = 0;
    barrier->num_threads = num_threads;
    barrier->generation = 0;
    pthread_mutex_init(&barrier->mutex, NULL);
    pthread_cond_init(&barrier->cond, NULL);
}

static void barrier_destroy(Barrier *barrier) 
{
    pthread_mutex_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->cond);
}

static void barrier_wait(Barrier *barrier) 
{
    pthread_mutex_lock(&barrier->mutex);
    int generation_ = barrier->generation;
    barrier->count++;
    if (barrier->count == barrier->num_threads) 
    {
        barrier->count = 0;
        barrier->generation++;
        pthread_cond_broadcast(&barrier->cond);
    } 
    else 
    {
        while (generation_ == barrier->generation)
            pthread_cond_wait(&barrier->cond, &barrier->mutex);
    }
    pthread_mutex_unlock(&barrier->mutex);
}

struct ThreadArg 
{
    int x;              
    int y;              
    int max_x;          
    int max_y;          
    int num_iter;       
    std::vector< std::vector<int> > *grid;  
    Barrier *barrier_calc;   
    Barrier *barrier_update; 
};

static void* cell_thread(void *arg) 
{
    ThreadArg *targ = static_cast<ThreadArg*>(arg);
    int x = targ->x;
    int y = targ->y;
    int max_x = targ->max_x;
    int max_y = targ->max_y;
    int num_iter = targ->num_iter;
    std::vector< std::vector<int> > *grid = targ->grid;

    for (int iter = 0; iter < num_iter; ++iter) 
    {
        int live_ = 0;
        for (int j = -1; j <= 1; ++j) 
        {
            for (int i = -1; i <= 1; ++i) 
            {
                if (i == 0 && j == 0)
                    continue; 
                int i2 = x + i;
                int j2 = y + j;
                if (i2 >= 0 && i2 < max_x && j2 >= 0 && j2 < max_y)
                {
                    if ((*grid)[j2][i2] == 1)
                        ++live_;
                }
            }
        }

        int current_ = (*grid)[y][x];
        int new_ = current_;
        if (current_ == 1)
        {
            if (live_ < 2 || live_ > 3)
                new_ = 0;
            else
                new_ = 1;
        } 
        else 
        {
            if (live_ == 3)
                new_ = 1;
            else
                new_ = 0;
        }
        barrier_wait(targ->barrier_calc);

        (*grid)[y][x] = new_;

        barrier_wait(targ->barrier_update);
    }
    return NULL;
}


std::vector< std::tuple<int,int> > run( std::vector< std::tuple<int,int> > initial_population,
                                          int num_iter, int max_x, int max_y )
{
    std::vector< std::vector<int> > grid(max_y, std::vector<int>(max_x, 0));
    int x = 0;
    int y = 0;
    for (size_t i = 0; i < initial_population.size(); ++i) 
    {
        x = std::get<0>(initial_population[i]);
        y = std::get<1>(initial_population[i]);
        if (x >= 0 && x < max_x && y >= 0 && y < max_y)
            grid[y][x] = 1;
    }

    int total_ = max_x * max_y;

    Barrier barrier_calc, barrier_update;
    barrier_init(&barrier_calc, total_);
    barrier_init(&barrier_update, total_);

    std::vector<pthread_t> threads(total_);
    std::vector<ThreadArg> thread_args(total_);

    int i = 0;
    for (int j = 0; j < max_y; ++j) 
    {
        for (int k = 0; k < max_x; ++k) 
        {
            thread_args[i].x = k;
            thread_args[i].y = j;
            thread_args[i].max_x = max_x;
            thread_args[i].max_y = max_y;
            thread_args[i].num_iter = num_iter;
            thread_args[i].grid = &grid;
            thread_args[i].barrier_calc = &barrier_calc;
            thread_args[i].barrier_update = &barrier_update;

            pthread_create(&threads[i], NULL, cell_thread, static_cast<void*>(&thread_args[i]));

            ++i;
        }
    }

    for (int i = 0; i < total_; ++i)
        pthread_join(threads[i], NULL);

    barrier_destroy(&barrier_calc);
    barrier_destroy(&barrier_update);

    std::vector< std::tuple<int,int> > answer_;
    for (int i = 0; i < max_y; ++i) 
    {
        for (int j = 0; j < max_x; ++j) 
        {
            if (grid[i][j] == 1) 
                answer_.push_back(std::make_tuple(j, i));
        }
    }
    return answer_;
}