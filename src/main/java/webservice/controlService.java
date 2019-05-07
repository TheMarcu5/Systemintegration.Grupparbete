/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package webservice;


import java.io.IOException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
@Path("/webservice")
/**
 *
 * @author marti
 */
public class controlService {
    sqlConnection con = new sqlConnection();
    
    @GET
    @Path("/currentstatus")
    @Produces(MediaType.APPLICATION_JSON)
    public List<sqlObject> allDataPoints() throws SQLException, IOException
    {
        List<sqlObject> objects = new ArrayList();
        objects = con.getValues();
      
        return objects;
    }
    
  
}
