
#if APIABSTRACTION_IOS
    // this is the code that will be called in actual main

    //params were set before this file's #include

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    //int retVal = UIApplicationMain(argc, argv, nil, nil);

    NSString *application =  NSStringFromClass([UIApplication class]);
    NSString *delegate =  NSStringFromClass([IOSAppDelegate class]);

    int retVal = UIApplicationMain(argc, argv, application,delegate);

    [pool release];
    return retVal;
} // end of main()
#endif
