//
//  GameViewController.h
//  battlebombs
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2014 Techne Games. All rights reserved.
//

#import <AppWarp_iOS_SDK/AppWarp_iOS_SDK.h>
#import "BaseGameViewController.h"

@interface GameViewController : BaseGameViewController <UITextFieldDelegate, ChatRequestListener, ConnectionRequestListener, LobbyRequestListener, NotifyListener, RoomRequestListener, ZoneRequestListener>
{
    // Nothing to do here
}

@property (strong, nonatomic) NSString *joinedRoomId;

@end
