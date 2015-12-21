class KeyGrabber
{
	static 
	{
		System.loadLibrary("KeyGrabber");
	}

	private native void listen();

	public static void fire_key_event()
	{
		System.out.println("key pressed (java code");
	}

	public static void main(String args[])
	{
		new KeyGrabber().listen();
	}
}
