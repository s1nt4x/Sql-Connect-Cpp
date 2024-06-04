## SQL Connect C++
This project explores interacting with an SQL database using C++ and ODBC.. It provides a simple interface for connecting to a database, executing queries, and handling results.
**This project has been created only for educational purposes.**

## Components
- **SqlDataBase Class:** Manages the database connection and handles error reporting.

- **Query Class:** Facilitates the preparation and execution of SQL statements.

## Usage
#### Set up database connection details
    SqlDataBase db;
    db.setHostName("your-server");
    db.setDataBaseName("your-database");
    db.setUserName("your-username");
    db.setPassword("your-password");
#### Open and close the connection
    db.open();
    db.close();
#### Prepare and execute different types of SQL queries
    Query query(db);
    query.prepare("your-query");
    query.exec();
#### Reset the query for reuse
    query.reset();
#### Display the results of the select query
    while (query.next()) {
    	for (int i = 0; i < query.getNumColumns(); i++)
    		std::cout << query.getColumnName(i) << " : " << query.value(i) << std::endl;
        std::cout << "***************" << std::endl;
    }
#### Example code
Watch the video in the repository

    #include "include/query.h"
    #include "include/sqldatabase.h"
    #include <iostream>
    
    int main() {
    
      // Set up database details
      SqlDataBase db;
      db.setHostName("your-server");       // Set your server name
      db.setDataBaseName("your-database"); // Set your database name
      db.setUserName("your-username");     // Set your database username
      db.setPassword("your-password");     // Set your database password
    
      // Open the database connection
      if (!db.open()) {
        return 1;
      }
    
      // Create a Query
      Query query(db);
    
      // Example of preparing and executing a query
      // Uncomment one of the following lines to prepare a specific type of query:
      // query.prepare("INSERT INTO your_table (column1, column2, ...) VALUES (?, ?, ...)");
      // query.prepare("UPDATE your_table SET your_column = ? WHERE your_condition");
      // query.prepare("DELETE FROM your_table WHERE your_condition");
    
      // Bind a parameter to the query (example with "NewValue")
      query.bindParameter(1, "NewValue");
    
      // Execute the query
      if (!query.exec()) {
        std::cerr << "Failed to execute query" << std::endl;
        return 2;
      }
    
      // Reset the query for reuse
      query.reset();
    
      // Display the results of the query
      while (query.next()) {
        for (int i = 0; i < query.getNumColumns(); i++)
          std::cout << query.getColumnName(i) << " : " << query.value(i) << std::endl;
        std::cout << "***************" << std::endl;
      }
    
      // Reset the query
      query.reset();
    
      // Close the database connection
      db.close();
    
      return 0;
    }
