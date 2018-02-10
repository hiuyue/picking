#version 330                                                                        

                                                                                                                                                                                                                                                       
uniform uint uModelIndex;                                                          
                                                                                    
out vec3 FragColor;

void main()                                                                         
{                                                                                   
   FragColor = vec3(float(uModelIndex), 0.0, 0.0);                 
}