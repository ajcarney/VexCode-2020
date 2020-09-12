/**
 * @file: ./RobotCode/src/objects/serial/Logger.hpp
 * @author: Aiden Carney
 * @reviewed_on: 2/9/2020
 * @reviewed_by: Aiden Carney
 *
 * contains class for a writer queue that accepts writes and flushes them
 * to an output stream
 */

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <atomic>
#include <queue>
#include <string>


typedef struct
{
    std::string stream;
    std::string content;
} log_entry;


/**
 * Contains a queue that can be added to and dumped out so that data can
 * be gathered and exported
 *
 */
class Logger
{
    private:
        static std::queue<log_entry> logger_queue;
        static std::atomic<bool> lock;

        /**
         * @param: num_entries -> max number of entries to get
         * @return: std::vector<log_entry> -> the list of items gotton from queue
         *
         * gets an object from the logger queue
         * returns an empty string if the queue is empty
         */
        std::vector<log_entry> get_entries(int num_entries);

        /**
         * @param: log_entry contents -> what to log
         * @return: bool -> true if the file was actually written to, false if an error occured
         *
         * sends an entry on a given stream
         * currently supports cout, clog, and cerr
         */
        bool log( log_entry entry );

    public:
        Logger();
        ~Logger();

        /**
         * @param: log_entry test_item -> item to add to the writer queue
         * @return: bool -> true on success and false if an error occured in the process
         *
         * adds an item to the logger queue
         * the queue is protected using a spinlock implemented with an
         * std::atomic bool
         */
        bool add( log_entry entry );

        /**
         * @return: None
         *
         * builds up a cache of items from the queue for 50ms so that they can be
         * logged at closer to the max speed
         */
        void dump( );



        /**
         * @return: int -> number of items in the logger queue
         *
         * returns the size of the logger queue
         */
        static int get_count();
};



#endif
