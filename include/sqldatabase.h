#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <Windows.h>
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

class SqlDataBase {
private:
  SQLHENV hEnv;
  SQLHDBC hDbc;
  bool connect;

  std::string databaseName;
  std::string hostName;
  std::string userName;
  std::string password;

  void showError(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE retCode) {
    SQLCHAR messageError[1024];
    SQLCHAR sqlState[6];
    SQLSMALLINT i = 1, length;
    SQLINTEGER nativeError;

    if (retCode == SQL_INVALID_HANDLE) {
      std::cerr << "Invalid Handle" << std::endl;
      return;
    }
    while (SQLGetDiagRec(hType, hHandle, i++, sqlState, &nativeError,
                         messageError, sizeof(messageError),
                         &length) == SQL_SUCCESS) {
      std::cerr << "SQL Error Message: " << messageError << std::endl;
      std::cerr << "SQL State: " << sqlState << std::endl;
    }
  }

public:
  SqlDataBase() : hEnv(nullptr), hDbc(nullptr), connect(false) {}

  ~SqlDataBase() { close(); }

  void setDataBaseName(const std::string &name) { databaseName = name; }
  void setHostName(const std::string &name) { hostName = name; }
  void setUserName(const std::string &user) { userName = user; }
  void setPassword(const std::string &pwd) { password = pwd; }

  bool open() {
    if (connect) {
      std::cerr << "There is already an open connection." << std::endl;
      return false;
    }

    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (!SQL_SUCCEEDED(ret)) {
      showError(hEnv, SQL_HANDLE_ENV, ret);
      return false;
    }

    ret =
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(ret)) {
      showError(hEnv, SQL_HANDLE_ENV, ret);
      SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
      return false;
    }

    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (!SQL_SUCCEEDED(ret)) {
      showError(hEnv, SQL_HANDLE_ENV, ret);
      SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
      return false;
    }

    std::string params = "DRIVER={SQL Server};SERVER=" + hostName +
                         ";DATABASE=" + databaseName + ";UID=" + userName +
                         ";PWD=" + password;

    ret = SQLDriverConnect(hDbc, nullptr, (SQLCHAR *)params.c_str(), SQL_NTS,
                           nullptr, 0, nullptr, SQL_DRIVER_COMPLETE);
    if (!SQL_SUCCEEDED(ret)) {
      showError(hDbc, SQL_HANDLE_DBC, ret);
      close();
      return false;
    }
    connect = true;
    return true;
  }

  bool isOpen() const { return connect; }

  void close() {
    if (hDbc != nullptr) {
      SQLDisconnect(hDbc);
      SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
      hDbc = nullptr;
    }
    if (hEnv != nullptr) {
      SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
      hEnv = nullptr;
    }
    connect = false;
  }

  SQLHDBC getConnectHandle() const { return hDbc; }
};

#endif // SQLDATABASE_H
