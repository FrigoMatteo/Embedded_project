package web;

import java.io.IOException;
import java.sql.ResultSet;
import java.sql.SQLException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class GetMinMax extends BLogicDB {

    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        // Ottieni il parametro 'type' dalla richiesta
        String type = request.getParameter("type");
        System.out.println("Type: " + type);

        serverConnection();

        // Esegui la logica aziendale in base al tipo
        String min_curr = "00";
        String min_hh = "00";
        String min_dd = "00";
        String min_mm = "00";
        String min_yy = "00";
        String max_curr = "00";
        String max_hh = "00";
        String max_dd = "00";
        String max_mm = "00";
        String max_yy = "00";
        
        try {
            // Switch basato sul tipo
            switch (type) {
                case "DD":
                    ResultSet hourResultSet = getHour("MIN");
                    ResultSet maxHourResultSet = getHour("MAX");

                    if (hourResultSet.next()) {
                        min_curr = hourResultSet.getString("current");
                        min_hh = hourResultSet.getString("hour");
                        min_dd = hourResultSet.getString("day");
                        min_mm = hourResultSet.getString("month");
                        min_yy = hourResultSet.getString("year");
                    }
                    if (maxHourResultSet.next()) {
                        max_curr = maxHourResultSet.getString("current");
                        max_hh = maxHourResultSet.getString("hour");
                        max_dd = maxHourResultSet.getString("day");
                        max_mm = maxHourResultSet.getString("month");
                        max_yy = maxHourResultSet.getString("year");
                    }
                    break;

                case "MM":
                    ResultSet dayResultSet = getDay("MIN");
                    ResultSet maxDayResultSet = getDay("MAX");

                    if (dayResultSet.next()) {
                        min_curr = dayResultSet.getString("current");
                        min_dd = dayResultSet.getString("day");
                        min_mm = dayResultSet.getString("month");
                        min_yy = dayResultSet.getString("year");
                    }
                    if (maxDayResultSet.next()) {
                        max_curr = maxDayResultSet.getString("current");
                        max_dd = maxDayResultSet.getString("day");
                        max_mm = maxDayResultSet.getString("month");
                        max_yy = maxDayResultSet.getString("year");
                    }
                    break;

                case "YY":
                    ResultSet monthResultSet = getMonth("MIN");
                    ResultSet maxMonthResultSet = getMonth("MAX");

                    if (monthResultSet.next()) {
                        min_curr = monthResultSet.getString("current");
                        min_mm = monthResultSet.getString("month");
                        min_yy = monthResultSet.getString("year");
                    }
                    if (maxMonthResultSet.next()) {
                        max_curr = maxMonthResultSet.getString("current");
                        max_mm = maxMonthResultSet.getString("month");
                        max_yy = maxMonthResultSet.getString("year");
                    }
                    break;

                case "AT":
                    ResultSet best_worstResultSet = getBestWorst();
                    if (best_worstResultSet.next()) {
                        min_curr = best_worstResultSet.getString("current");
                        min_mm = best_worstResultSet.getString("month");
                        min_yy = best_worstResultSet.getString("year");
                        max_curr = best_worstResultSet.getString("current");
                        max_mm = best_worstResultSet.getString("month");
                        max_yy = best_worstResultSet.getString("year");
                    }
                    break;

                default:
                    // Not valid type
                    response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
                    return;
            }
        } catch (SQLException e) {
            // Gestione dell'errore SQL
            e.printStackTrace();
            response.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
        } finally {
            // Chiudi la connessione al database
            if (conn != null) {
                try {
                    conn.close();
                } catch (SQLException e) {
                    e.printStackTrace();
                }
            }
        }

        // Crea una risposta JSON
        String jsonResponse = "{ " +
                "\"min_curr\": \"" + min_curr + "\", " +
                "\"min_hh\": \"" + min_hh + "\", " +
                "\"min_dd\": \"" + min_dd + "\", " +
                "\"min_mm\": \"" + min_mm + "\", " +
                "\"min_yy\": \"" + min_yy + "\", " +
                "\"max_curr\": \"" + max_curr + "\", " +
                "\"max_hh\": \"" + max_hh + "\", " +
                "\"max_dd\": \"" + max_dd + "\", " +
                "\"max_mm\": \"" + max_mm + "\", " +
                "\"max_yy\": \"" + max_yy + "\" }";

        // Imposta il tipo di contenuto e scrivi la risposta
        response.setContentType("application/json");
        response.getWriter().print(jsonResponse);

    }

    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        doGet(request, response);
    }
}
