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
  // query.prepare("INSERT INTO your_table (column1, column2, ...) VALUES (?, ?,
  // ...)"); query.prepare("UPDATE your_table SET your_column = ? WHERE
  // your_condition"); query.prepare("DELETE FROM your_table WHERE
  // your_condition");

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
      std::cout << query.getColumnName(i) << " : " << query.value(i)
                << std::endl;
    std::cout << "***************" << std::endl;
  }

  // Reset the query
  query.reset();

  // Close the database connection
  db.close();

  return 0;
}
