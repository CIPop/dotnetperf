<%@ WebHandler Language="C#" Class="Echo" %>

using System;
using System.Web;
using System.IO;

public class Echo : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        if (context.Request.HttpMethod == "POST")
        {
            context.Request.InputStream.CopyTo(context.Response.OutputStream);
        }   
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}