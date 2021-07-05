## Introduction
ToDoGoal – is a regular to do list writing on c++ and Qt, but there is a database connected to this app. Here I use the PostgreSQL to modify a database. Also you have a possibility to change language of the interface. Four languages are available: English, German, Russian and Ukrainian.

## Installation guide
1)	First of all, you need to download this repository. You can use the following command:
```
git clone https://github.com/goal-action/ToDoGoal.git
```
2)	Then open your SQL Shell (psql) and create a new database:
```
create database <name>;
```
3)	After that, you need to restore the database using the dump file in the repository that you downloaded. Open your PowerShell, go to your Postgres bin folder and run the command:
```
psql -U <your_user_name> -d <your_data_base_name> -f <path_to_dumpfile>
```
<your_data_base_name> - this is the name of the database you created in step 2.

for example the name of my database is “todogoal”, so command for me:
```
psql –U postgres –d todogoal –f D:/ToDoGoal/dump/todogoalDump.sql
```

run it and type password for your postgres user.

4)	Lastly, open maintaskswidget.cpp file and change arguments for functions setHostName, setDataBaseName, setUserName and setPassword according to your settings:
```
9     m_dataBase.setHostName("127.0.0.1");
10    m_dataBase.setDatabaseName("todogoal");
11    m_dataBase.setUserName("postgres");
12    m_dataBase.setPassword("goalaction");
```
5) Then you can run the program!
