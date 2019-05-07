
package webservice;


import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;


public class sqlConnection {

   
    public sqlConnection()
    {
        try{
        Class.forName("com.mysql.cj.jdbc.Driver");
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }
    
    public List<sqlObject> getValues() throws SQLException, FileNotFoundException, IOException
    {
        
        sqlObject datapoint = new sqlObject();
        List<sqlObject> datapointList =  new ArrayList<>();
        Properties p = new Properties();
        p.load(new FileInputStream("C:\\Users\\marti\\Documents\\NetBeansProjects\\webserviceIn2\\src\\main\\java\\webservice\\sqlproperies.properties"));
           try (        
            Connection con = DriverManager.getConnection(p.getProperty("connectionString"), p.getProperty("name"), p.getProperty("password"));
                        PreparedStatement stmt = con.prepareStatement("SELECT * FROM Data");)
                        {
                        ResultSet rs = stmt.executeQuery();     
            while (rs.next()) {
                datapoint = new sqlObject();
                datapoint.setHallId(rs.getInt("DeviceID"));
                datapoint.setTemperature(rs.getFloat("Temperature"));
                datapointList.add(datapoint);
            }
        }
        catch (Exception e){
            e.printStackTrace();
        }
        return datapointList;
    }
    

}   
