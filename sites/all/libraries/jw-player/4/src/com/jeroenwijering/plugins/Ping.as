﻿package com.jeroenwijering.plugins {			import com.jeroenwijering.events.*;	import com.jeroenwijering.utils.Logger			import flash.display.MovieClip;	import flash.external.ExternalInterface;	import flash.net.sendToURL;	import flash.net.URLRequest;	import flash.utils.setTimeout;			/**	 * Ping Plugin; pings a serverside script whenever a video starts, seeks, stops, completes or unloads.	 **/	public class Ping extends MovieClip implements PluginInterface {						/** List with configuration settings. **/		public var config:Object = {			script:undefined		};				/** Page referrer. **/		private var _referrer:String;		/** Reference to the _view of the player. **/		private var _view:AbstractView;		/** Start time **/		private var _startTime:Number = -1;		/** Last time tick **/		private var _lastTime:Number = -1;						/** Constructor; nothing going on. **/		public function Ping():void {};						/** The initialize call is invoked by the player _view. **/		public function initializePlugin(view:AbstractView):void {			_view = view;						_view.addControllerListener(ControllerEvent.ITEM,itemHandler);			_view.addControllerListener(ControllerEvent.SEEK,seekHandler);			_view.addControllerListener(ControllerEvent.STOP,stopHandler);						_view.addModelListener(ModelEvent.STATE, stateHandler);			_view.addModelListener(ModelEvent.TIME,timeHandler);						addBrowserNavigationNotification();			if(ExternalInterface.available) {				try { 					_referrer = ExternalInterface.call("function(){return window.location.href;}");				} catch(err:Error) {}			}		};						/** Event is caught; ping the server. **/		private function itemHandler(evt:ControllerEvent):void {			// Nasty timeout to allow the player to retrieve it's DOM ID.			setTimeout(sendPing,500,'item');			_startTime = -1;			_lastTime = -1;		};						/** Event is caught; ping the server. **/		private function seekHandler(evt:ControllerEvent):void {			sendPing('seek', _startTime, _lastTime - _startTime);			_startTime = -1;			_lastTime = -1;		}								/** Event is caught; ping the server. **/		private function stopHandler(evt:ControllerEvent):void {			sendPing('stop', _startTime, _lastTime - _startTime);			_startTime = -1;			_lastTime = -1;		}						/** Event is caught; ping the server. **/		private function stateHandler(evt:ModelEvent):void {			if (evt.data.newstate == ModelStates.COMPLETED) {				sendPing('complete', _startTime, _lastTime - _startTime);				_startTime = -1;				_lastTime = -1;			}		}						/** Event is caught; update the time **/		private function timeHandler(evt:ModelEvent):void {			if (_startTime == -1) {				_startTime = evt.data.position;			}			if (!((_lastTime > 0) && (evt.data.position == 0))){				_lastTime = evt.data.position;			}		}						/**		 * This function tells the web browser to notify the player of a navigation		 * event. This function runs in addtion to the code already on the page.		 **/		private function addBrowserNavigationNotification():void{			try{				// Exposes a function 'navigationAlert()' to the browser's JavaScript evaluator				ExternalInterface.addCallback("navigationAlert", unloadHandler);								/**				 * Code for Internet Explorer.				 * - Runs the JavaScript eval() command to create a function 'playerCallback',				 *   which grabs the player element, calls the 'navigationAlert()' function,				 *   and waits 1 second.				 * - Tells the window to call 'playerCallback(evt)' before navigating away by				 *   attaching an event listener to the window. This will not override any				 *   existing 'onBeforeUnload()' functions.				 * - Everything is wrapped in a try / catch block because it will fail for				 *   non-IE browsers.				 **/				ExternalInterface.call("function() {try {eval(function playerCallback(evt){document.getElementById('"+ExternalInterface.objectID+"').navigationAlert();setTimeout('true;',1000);});var r = window.attachEvent('onbeforeunload', playerCallback);return r;} catch(err){}}");								/**				 * Code for Firefox / Safari / Chrome.				 * - Tells the window to call an anonymous function before navigating away by				 *   attaching an event listener to the window. (This will not override any				 *   existing 'onBeforeUnload()' functions.) The anonymous function grabs the				 *   player element, calls the 'navigationAlert()' function, and waits 1 second.				 * - Everything is wrapped in a try / catch block because it will fail for IE.				 **/				ExternalInterface.call("function() {try {window.addEventListener('beforeunload',function(){document.getElementById('"+ExternalInterface.objectID+"').navigationAlert();setTimeout('true;',1000);},false);} catch(err) {}}");			} catch (e:Error){				Logger.log("Error adding browser navigation callback: "+e);			}		}						/** This function is called when a browser navigation event occurs **/		private function unloadHandler():void {			if (_startTime != -1) {				sendPing('unload', _startTime, _lastTime - _startTime);			}		}						/** Wrap up the url generation and do the ping. **/		private function sendPing(eventType:String, start:Number = -1, duration:Number = -1):void {			var prm = 'event='+eventType;			if ((start >= 0) && (duration >= 0)) {				prm += '&start=' + Math.round(start*10) / 10 + '&duration=' + Math.round(duration*10) / 10;			} else if (eventType != 'item') { 				return;			}			prm += '&file='+encodeURIComponent(_view.playlist[_view.config['item']]['file']);			prm += '&client='+encodeURIComponent(_view.config['client']);			prm += '&id='+encodeURIComponent(_view.config['id']);			if(eventType == 'item') {				prm += '&referrer='+encodeURIComponent(_referrer);			}			prm += '&version='+_view.config['version'];			prm += '&rand='+Math.random();			Logger.log(prm,'ping');			if(config['script']) {				Logger.log('PING:'+config['script']+'?'+prm);				sendToURL(new URLRequest(config['script']+'?'+prm));			}		};					};		}