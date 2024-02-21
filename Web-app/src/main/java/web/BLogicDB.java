package web;

import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class BLogicDB extends HttpServlet {

    String dbURL = "jdbc:postgresql://localhost:5432/embedded";
    String user = "postgres";
    String password = "postgres";
    Connection conn = null;

    public void serverConnection() {
        try {
            // Class.forName("org.apache.derby.jdbc.ClientDriver");
            Class.forName("org.postgresql.Driver");
            conn = DriverManager.getConnection(dbURL, user, password);
            System.out.println("DB connection: " + conn);
        } catch (ClassNotFoundException | SQLException ex) {
            System.out.println(ex);
        }
    }

    protected ResultSet getHour(String x) throws SQLException {
        String query = "SELECT current, " +
                "    hour, " +
                "    DATE_PART('day', date) AS day, " +
                "    DATE_PART('month', date) AS month, " +
                "    DATE_PART('year', date) AS year " +
                "FROM last_day " +
                "WHERE (current, date) = (" +
                "    SELECT current, date " +
                "    FROM last_day " +
                "    WHERE current = (" +
                "        SELECT " + x + "(current) " +
                "        FROM last_day)" +
                "    ORDER BY date DESC " +
                "    LIMIT 1);";
        PreparedStatement stmt = conn.prepareStatement(query);
        return stmt.executeQuery();
    }

    protected ResultSet getDay(String x) throws SQLException {
        String query = "SELECT current, " +
                "    DATE_PART('day', date) AS day, " +
                "    DATE_PART('month', date) AS month, " +
                "    DATE_PART('year', date) AS year " +
                "FROM last_month " +
                "WHERE (current, date) = (" +
                "    SELECT current, date " +
                "    FROM last_month " +
                "    WHERE current = (" +
                "        SELECT " + x + "(current) " +
                "        FROM last_month)" +
                "    ORDER BY date DESC " +
                "    LIMIT 1);";
        PreparedStatement stmt = conn.prepareStatement(query);
        return stmt.executeQuery();
    }

    protected ResultSet getMonth(String x) throws SQLException {
        String query = "SELECT current, month, year " +
                       "FROM last_year " +
                       "WHERE (current, year, month) = (" +
                       "    SELECT current, year, month " +
                       "    FROM last_year " +
                       "    WHERE current = (" +
                       "        SELECT " + x + "(current) " +
                       "        FROM last_year)" +
                       "    ORDER BY year DESC, month DESC " +
                       "    LIMIT 1);";
        PreparedStatement stmt = conn.prepareStatement(query);
        return stmt.executeQuery();
    }
    

    protected ResultSet getBestWorst() throws SQLException {
        String query = "SELECT current, month, year " +
                "FROM best_worst_table " +
                "ORDER BY best_worst ASC";
        PreparedStatement stmt = conn.prepareStatement(query);
        return stmt.executeQuery();
    }

    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
    }

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }

    public void destroy() {
        try {
            conn.close();
        } catch (SQLException ex) {
            ex.printStackTrace();
        }
    }

}
