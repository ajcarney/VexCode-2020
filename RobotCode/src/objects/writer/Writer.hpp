/**
 * @file: ./RobotCode/src/objects/writer/Writer.hpp
 * @author: Aiden Carney
 * @reviewed_on: 12/4/19
 * @reviewed_by: Aiden Carney
 * TODO: instead of writing to sd card, flush to serial because sd card is not good
 *
 * contains class for a writer queue that accepts writes and flushes them
 * to an output stream
 */

#ifndef __WRITER_HPP__
#define __WRITER_HPP__

#include <atomic>
#include <list>
#include <queue>
#include <string>
#include <unordered_map>


//#define MACRO_RECORD_DEBUG
#define BUFFER_SIZE 256


typedef struct
{
    std::string file;
    std::string mode;
    std::string content;
} writer_obj;



/**
 * Contains a queue that can be added to and dumped out so that data can
 * be gathered and exported
 * contains other implementation for interacting with the SD card
 */
class Writer
{
    private:
        static std::queue<writer_obj> write_queue;
        static std::atomic<bool> lock;

        static int num_files_open;
        static std::string match_file_name;

        /**
         * @return: writer_obj
         *
         * gets an object from the writer queue
         * returns an uninitialized writer_obj if the queue is empty
         */
        writer_obj get_write( );

        /**
         * @param: std::string file_name -> location of the file
         * @param: std::string mode -> mode to open file in (a for append, w for write)
         * @param: std::string contents -> what to write to the file
         * @return: bool -> true if the file was actually written to, false if an error occured
         *
         * uses the c api to take a big string and write BUFFER_SIZE chars to
         * file at a time
         *
         * currently writes to a big match file that would have to be parsed
         * segfault occurs when writing with errno 5, so this command should not
         * be used in a match
         */
        bool write( std::string file_name, std::string mode, std::string contents );

    public:
        Writer();
        ~Writer();

        /**
         * @param: writer_obj test_item -> item to add to the writer queue
         * @return: None
         *
         * adds an item to the writer queue
         * the queue is protected using a spinlock implemented with an
         * std::atomic bool
         */
        void add( writer_obj test_item );

        /**
         * @return: bool -> true on success and false if an error occured in the process
         *
         * builds up a cache of items from teh queue for 50ms so that they can be
         * written closer to the max file write speed
         * currently uses private write method which will segfault under high loads
         * do not use during a competition
         */
        bool dump( );




        /**
         * @param: std::string prev_name -> name of the file to change the name of
         * @param: std::string new_name -> new name for the file
         * @return: None
         *
         * coes not actually rename the file as that has not been implented in the
         * vex os or pros api
         * instead writes contents of current file to the file of the new name
         */
        void rename( std::string prev_name, std::string new_name);

        /**
         * @param: std::string file_name
         * @return: None
         *
         * coes not actually remove the file as that has not been implented in the
         * vex os or pros api
         * instead opens the file in truncate mode to clear the contens of the file
         * allows for mock sd card functionality
         */
        void remove( std::string file_name );




        /**
         * @return: int -> number of items in the writer queue
         *
         * returns the size of the writer queue
         */
        static int get_count();

        /**
         * @return: int -> number of files tracked to be open
         *
         * returns the number of files open
         * this is not a full proof method as this is only kept track of
         * when a file is opened using the write method in the class
         * because of this, this is mainly to be used for debugging
         */
        static int get_num_files_open();
};



#endif
