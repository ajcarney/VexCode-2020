/**
 * @file: ./RobotCode/src/objects/writer/Writer.cpp
 * @author: Aiden Carney
 * @reviewed_on: 12/5/19
 * @reviewed_by: Aiden Carney
 *
 * @see Writer.hpp
 *
 * contains implementation for the writer class
 */

#include <atomic>
#include <chrono>
#include <ctime>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <sstream>
#include <string>
#include <assert.h>
#include <iostream>
#include <cmath>
#include <cerrno>
#include <cstring>
#include <clocale>


#include "../../../include/main.h"

#include "../../../lib/date/date.h"
#include "Writer.hpp"

std::queue<writer_obj> Writer::write_queue;
std::atomic<bool> Writer::lock = ATOMIC_VAR_INIT(false);
int Writer::num_files_open = 0;
std::string Writer::match_file_name = "/usd/match_data/" + date::format("%d-%m-%Y", date::floor<std::chrono::microseconds>(std::chrono::system_clock::now())) + ".match";


Writer::Writer() { }




Writer::~Writer() { }




/**
 * formats content to be written to a match file to avoid having multiplte files open
 * writes data by making chunks of 256 bytes
 * uses the c api and performs checks to see where errors occur
 * typically sets errno to 5: too many files open in system even though only
 * one file has ever been opened
 */
bool Writer::write( std::string file_name, std::string mode, std::string contents )
{
    std::string to_write = "{'filename':'" + file_name + "','mode':'" + mode + "','contents':";
    int i = 0;
    FILE *f;
    int size;
    int written;

    f = fopen(match_file_name.c_str(), "a");
    if ( f == NULL )  //check file was opened correctly
    {
        std::cout << "not opened correctly\n" << std::flush;
        std::cout << "errno: " << errno << "\n" << std::flush;
        std::cout << "strerror: " << std::strerror(errno) << "\n" << std::flush;
        std::cout << match_file_name.c_str() << "\n" << std::flush;
        pros::delay(10);
        fclose(f);
        return false;  //nothing was written
    }
    num_files_open += 1;


    //finds all '\n' characters and replaces with /*eol*/ so it can be parsed more consistently
    std::string to = "/*eol*/";
    std::string from = "\n";
    size_t start_pos = 0;
    while((start_pos = contents.find(from, start_pos)) != std::string::npos) {
        contents.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    to_write += contents + "'}\n";


    size = to_write.size();
    written = 0;
    while ( written < size )
    {
        char tempBuf[BUFFER_SIZE];

        for( i=0; i<(BUFFER_SIZE - 1); i++ )  //use BUFFER_SIZE - 1 so that terminator can be added
        {
            if ( written >= (size) )  //use ">=" so that no characters past the size of the string will be printed
            {
                break;
            }
            tempBuf[i] = to_write.at(written);
            written += 1;
        }
        tempBuf[i] = '\0';

        std::cout << "writing to file\n";

        int ret = fputs(tempBuf, f);  //write to file
        std::cout << "errno: " << errno << "\n" << std::flush;
        std::cout << "strerror: " << std::strerror(errno) << "\n" << std::flush;
        pros::delay(6);
        fflush(f);

        //int ret = fwrite ( tempBuf, sizeof(char), sizeof(tempBuf), f);
        if ( ret == EOF )
        {
            std::cout << "not written correctly\n" << std::flush;
            std::cout << "errno: " << errno << "\n" << std::flush;
            std::cout << "strerror: " << std::strerror(errno) << "\n" << std::flush;
            std::cout << file_name.c_str() << "\n" << std::flush;
            pros::delay(10);
            fclose(f);
            return false;  //operation failed
        }
        //std::cout << "items written: " << written << "  items to write: " << size << "  items left: " << (size - written) << "\n";
    }

    std::cout << "closing file\n";
    pros::delay(5);

    fflush(f);
    int ret = fclose(f);
    if ( ret == EOF )
    {
        std::cout << "not closed correctly\n" << std::flush;
        std::cout << "errno: " << errno << "\n" << std::flush;
        std::cout << "strerror: " << std::strerror(errno) << "\n" << std::flush;
        std::cout << file_name.c_str() << "\n" << std::flush;
        pros::delay(10);
    }
    else
    {
        num_files_open -= 1;
    }
    std::cout << "finished\n";
    std::cout << "errno: " << errno << "\n" << std::flush;
    std::cout << "strerror: " << std::strerror(errno) << "\n" << std::flush;
    return true;

}




/**
 * add item to the writer queue by aquiring and releasing atomic lock
 */
void Writer::add( writer_obj test_item )
{
    if ( !test_item.file.empty() && !test_item.mode.empty() )
    {
        while ( lock.exchange( true ) ); //aquire lock
        write_queue.push( test_item );
        lock.exchange( false ); //release lock
    }
    else
    {
        std::cout << "adding failed********************\n";
    }
}




/**
 * gets an item from the queue by acquiring the lock and releasing it
 */
writer_obj Writer::get_write( )
{
    writer_obj contents;

    //checks if there are items to be written, useful so that this function is
    //not blocking if the queue is empty
    if ( !write_queue.empty() )
    {
        while ( lock.exchange( true ) ); //aquire lock
        contents = write_queue.front();
        write_queue.pop();
        lock.exchange( false ); //release lock because there is no more iteraction
                                //with the queue

    }
    return contents;
}




/**
 * builds up a cache of items based on the file name and writes it in bulk
 * the items are stored in vectors based on the file name
 * this is used so that the max write speed can be used and less time is spent
 * waiting
 */
bool Writer::dump( )
{
    std::vector< writer_obj > writes;
    std::vector< writer_obj > temp_vec;

    writes.reserve(100000);
    temp_vec.reserve(100000);

    int stop = pros::millis() + 50;
    //for ( int i = 0; i < 200; i++ )  //collects up to 200 writer objects
    while ( pros::millis() < stop )
    {
        writer_obj contents = get_write();

        if ( !contents.file.empty() && !contents.mode.empty() )
        {
/*            std::ofstream out_file( contents.file, std::ios_base::app ); //dump current contents
            num_files_open += 1;
            out_file << contents.content;
            out_file.close();
            num_files_open -= 1;*/
            writes.push_back( contents );
        }
        else //object returned was empty meaning queue is empty
             //it would be wasteful to look at the queue when it is known to be empty
        {
            pros::delay(1);
            //break;
        }
    }

    if ( writes.empty() )
    {
        return false;
    }

    while ( !writes.empty() )
    {
        std::string write_loc;
        std::string write_mode;
        std::string write_contents;
        #ifdef MACRO_RECORD_DEBUG
            std::string write_contents_debug;
            std::string delimiter = ",";
        #endif

        write_contents.reserve(2500);
        #ifdef MACRO_RECORD_DEBUG
            write_contents_debug.reserve(10000);
        #endif


        int first = 0;
        while ( write_mode.empty() && ( first < writes.size() ) )  //iterate until the first element is not of mode write
                                                                   //algorithm writes mode "w" immediately
        {
            if ( writes.at(first).mode.compare("a") == 0 )
            {
                write_loc = writes.at(first).file;
                write_mode = writes.at(first).mode;
                write_contents = writes.at(first).content;
                #ifdef MACRO_RECORD_DEBUG
                    write_contents_debug = (writes.at(first).content.substr(0, writes.at(first).content.find(delimiter))
                        + ","
                        + date::format("%d-%m-%Y %T", date::floor<std::chrono::microseconds>(std::chrono::system_clock::now()))
                        + "\n");
                #endif
                break;
            }
            else
            {
                write(writes.at(first).file, "w", writes.at(first).content);
            }
            first += 1;
        }


        for ( int i = first; i < writes.size(); i++ )
        {
            //case 1: the object in question is mode append and is the file that is being
            //searched for
            if ( writes.at(i).file.compare(write_loc) == 0 && writes.at(i).mode.compare("a") == 0 )
            {
                write_contents += writes.at(i).content;
                #ifdef MACRO_RECORD_DEBUG
                    write_contents_debug += (writes.at(i).content.substr(0, writes.at(i).content.find(delimiter))
                        + ","
                        + date::format("%d-%m-%Y %T", date::floor<std::chrono::microseconds>(std::chrono::system_clock::now()))
                        + "\n");
                #endif
            }
            //case 2: the object in question is the file that is being searched for but is of
            //mode write
            else if ( writes.at(i).file.compare(write_loc) == 0 && writes.at(i).mode.compare("w") == 0 )
            {
                write(write_loc, "a", write_contents);

                #ifdef MACRO_RECORD_DEBUG
                    write("/usd/log/15/from_queue/data.csv", "a", write_contents_debug);
                #endif

                write(write_loc, "w", writes.at(i).content);

                write_loc.clear();
                write_mode.clear();
                write_contents.clear();

                break;
            }
            //case 3 the object in question is not of the same file
            else
            {
                temp_vec.push_back( writes.at(i) );
            }

        }
/*        //std::cout << write_contents << "\n\n";
        std::ofstream out_file( write_loc, std::ios_base::app ); //dump current contents
        num_files_open += 1;
        if ( out_file.good() )
        {
            out_file << write_contents;
        }
        else
        {
            std::cout << "file not opened correctly\n";
        }
        out_file.close();
        num_files_open -= 1;*/
        //std::cout << "starting write\n";
        bool ret = write(write_loc, "a", write_contents);
        if (!ret)
        {
            std::cout << "write failed\n";
            pros::delay(5);
        }
        //std::cout << "write finished\n";

        #ifdef MACRO_RECORD_DEBUG
            write("/usd/log/15/from_queue/data.csv", "a", write_contents_debug);
        #endif

        writes.clear();  //swap contents of both lists

        std::cout << writes.size() << " " << temp_vec.size() << "\n";

        for ( int i = 0; i < temp_vec.size(); i++ )
        {
            writes.push_back( temp_vec.at(i) );
        }
        temp_vec.clear();

    }

    return true;

}




/**
 * opens file and copies it to the new file name so that is appears to have
 * been renamed
 * also functions as a file move operation
 */
void Writer::rename( std::string prev_name, std::string new_name)
{
    //perform operation with lock aquired because it is a copy
    //rename
    while ( lock.exchange( true ) ); //aquire lock

    std::ifstream src;
    std::ofstream dst;
    num_files_open += 1;

    src.open(prev_name, std::ios::in);
    dst.open(new_name, std::ios::app);

    dst << src.rdbuf();

    src.close();
    num_files_open -= 1;
    dst.close();

    lock.exchange( false ); //release lock
}




/**
 * simulates removing a file by opening it in truncate mode
 * which clears the contents of the file
 */
void Writer::remove( std::string file_name )
{
    //perform operation with lock aquired because it is a clear contents
    //delete
    while ( lock.exchange( true ) ); //aquire lock

    std::ofstream file( file_name, std::ios::out | std::ios::trunc ); // clear contents
    num_files_open += 1;
    file.close( );
    num_files_open -= 1;

    lock.exchange( false ); //release lock
}




/**
 * gets the size of the writer queue
 */
int Writer::get_count()
{
    return write_queue.size();
}




/**
 * gets the number of files opened that has been tracked by methods in the class
 */
int Writer::get_num_files_open()
{
    return num_files_open;
}
