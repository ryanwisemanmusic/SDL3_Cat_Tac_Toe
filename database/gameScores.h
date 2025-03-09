#ifndef GAMESCORES
#define GAMESCORES

#include <sqlite3.h>
#include <string>
#include <vector>

struct ScoreEntry
{
    int id;
    std::string player_name;
    int score;
    std::string timestamp;
};

class DatabaseManager
{
    public:
    std::string dbFile;
    DatabaseManager(const std::string& dbFile);
    ~DatabaseManager();
    void queryScores();

    bool insertTestScore(const std::string& player_name, int score);
    int DatabaseManager::callback(void* NotUsed, int argc, char** argv, char** azColName);

    private:
    sqlite3* db;
    bool executeSQL(const std::string& sql);
};

#endif