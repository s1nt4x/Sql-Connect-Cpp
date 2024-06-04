#ifndef QUERY_H
#define QUERY_H

#include "sqldatabase.h"

class Query {
private:
  SQLHSTMT hStm;
  SQLHDBC hDbc;

  bool executed;
  int numColumns;

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
  Query(SqlDataBase &db)
      : hStm(nullptr), hDbc(db.getConnectHandle()), executed(false),
        numColumns(0) {
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStm);
    if (!SQL_SUCCEEDED(ret)) {
      showError(hDbc, SQL_HANDLE_DBC, ret);
      hStm = nullptr;
    }
  }

  ~Query() {
    if (hStm != nullptr) {
      SQLFreeHandle(SQL_HANDLE_STMT, hStm);
    }
  }

  bool prepare(const std::string &query) {
    if (hStm == nullptr) {
      std::cerr << "Statement handle not allocated" << std::endl;
      return false;
    }

    SQLRETURN ret = SQLPrepare(hStm, (SQLCHAR *)query.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
      showError(hStm, SQL_HANDLE_STMT, ret);
      return false;
    }

    ret = SQLNumResultCols(hStm, (SQLSMALLINT *)&numColumns);
    if (!SQL_SUCCEEDED(ret)) {
      showError(hStm, SQL_HANDLE_STMT, ret);
      return false;
    }

    executed = false;
    return true;
  }

  bool exec() {
    if (hStm == nullptr) {
      std::cerr << "Statement handle not allocated or query not prepared"
                << std::endl;
      return false;
    }

    SQLRETURN ret = SQLExecute(hStm);
    if (!SQL_SUCCEEDED(ret)) {
      showError(hStm, SQL_HANDLE_STMT, ret);
      return false;
    }

    executed = true;
    return true;
  }

  bool next() {
    if (hStm == nullptr || !executed) {
      std::cerr << "Query not executed" << std::endl;
      return false;
    }

    SQLRETURN ret = SQLFetch(hStm);
    if (ret == SQL_NO_DATA) {
      return false;
    } else if (!SQL_SUCCEEDED(ret)) {
      showError(hStm, SQL_HANDLE_STMT, ret);
      return false;
    }

    return true;
  }

  int getNumColumns() const { return numColumns; }

  std::string value(int index) {
    char columnBuff[256];
    SQLLEN indicator;

    if (hStm == nullptr || !executed) {
      std::cerr << "Query not executed" << std::endl;
      return "";
    }

    SQLRETURN ret = SQLGetData(hStm, index + 1, SQL_C_CHAR, columnBuff,
                               sizeof(columnBuff), &indicator);
    if (!SQL_SUCCEEDED(ret)) {
      showError(hStm, SQL_HANDLE_STMT, ret);
      return "";
    }

    if (indicator == SQL_NULL_DATA) {
      return "";
    } else {
      return std::string(columnBuff, indicator);
    }
  }

  bool bindParameter(int index, const std::string &value) {
    if (hStm == nullptr) {
      std::cerr << "Statement handle not allocated" << std::endl;
      return false;
    }

    SQLRETURN ret = SQLBindParameter(
        hStm, index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, value.size(), 0,
        (SQLPOINTER)value.c_str(), value.size(), nullptr);
    if (!SQL_SUCCEEDED(ret)) {
      showError(hStm, SQL_HANDLE_STMT, ret);
      return false;
    }
    return true;
  }

  std::string getColumnName(int index) {
    SQLCHAR columnName[256];
    SQLSMALLINT nameLength;

    if (hStm == nullptr) {
      std::cerr << "Statement handle not allocated" << std::endl;
      return "";
    }

    SQLRETURN ret =
        SQLDescribeCol(hStm, index + 1, columnName, sizeof(columnName),
                       &nameLength, nullptr, nullptr, nullptr, nullptr);

    if (!SQL_SUCCEEDED(ret)) {
      std::cerr << "Error obtaining column name " << index << std::endl;
      showError(hStm, SQL_HANDLE_STMT, ret);
      return "";
    }

    return std::string((char *)columnName, nameLength);
  }

  void reset() {
    if (hStm != nullptr) {
      SQLFreeStmt(hStm, SQL_CLOSE);
    }
    executed = false;
  }
};

#endif // QUERY_H
