if (!server.IsAboutToStart())
{
	//engine.ImportExtension("qt.core");
    //engine.ImportExtension("qt.gui");
    
	sceneAPI = framework.Scene();

    client.createOgre.connect(CreateScenemanager);
	client.deleteOgre.connect(DeleteScenemanager);
	client.setOgre.connect(SetScenemanager);

	function CreateScenemanager(sceneName)
	{
		framework.renderer.CreateSceneManager(sceneName);
		framework.renderer.SetSceneManager(sceneName);
		print("Created new Ogre scenemanager " + sceneName);
	}

	function DeleteScenemanager(sceneName)
	{
		framework.renderer.SetSceneManager("SceneManager");
		framework.renderer.RemoveSceneManager(sceneName);
		print("Deleted Ogre scenemanager " + sceneName);
	}	
	
	function SetScenemanager(sceneName)
	{
		framework.renderer.SetSceneManager(sceneName);
		print("Changed Ogre scenemanager to " + sceneName);
	}	
}