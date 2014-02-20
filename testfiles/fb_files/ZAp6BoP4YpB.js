/*!CK:1322274240!*//*1392693963,178183213*/

if (self.CavalryLogger) { CavalryLogger.start_js(["wxq+C"]); }

__d("SnowliftPicCropper",["Arbiter","CSS","Dialog","DOM","Event","Form","Keys","Parent","Photocrop2","PhotosConst","ProfilePicRequestCreator","ProfilePictureFlowLogging","Style","URI","copyProperties","tx"],function(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v){function w(x){"use strict";this.root=x.getRoot();this.photoSnowlift=x;}w.prototype.init=function(){"use strict";this.croppingMode=false;this.optionMenu=j.find(this.root,'div.fbPhotoSnowliftContainer');this.overlayActions=j.find(this.root,'div.snowliftOverlayBar');this.setupHandlers();g.subscribe('PhotoTagger.REMOVED_MAKE_PROFILE_PIC_OPTION',this.disableCropping.bind(this,false));};w.prototype.setupHandlers=function(){"use strict";this.handlers=[k.listen(this.optionMenu,'click',this.clickHandler.bind(this)),k.listen(this.overlayActions,'click',this.clickHandler.bind(this)),k.listen(window,'resize',(function(event){this.resetPhoto();}).bind(this)),k.listen(document.documentElement,'keydown',function(event){if(!this.croppingMode)return;var x=k.getKeyCode(event);if(x===m.ESC){this.disableCropping(false);k.kill(event);}}.bind(this))];};w.prototype.submitCroppedPhoto=function(){"use strict";var x=this.photoSnowlift.getCurrentPhotoInfo();if(!x)return;r.log(r.step.LOADING);var y=this.getProfilePicTargetId(x);h.addClass(this.root,'profilePicSavingMode');var z=this.disableCropping(true);new q(x.fbid,x.owner,y,z).setSuccessURI(new t('/profile.php').setQueryData({id:y}).toString()).setErrorURI(new t('/photo.php').setQueryData({pid:x.pid,id:x.owner}).toString()).setIsUnscaled(true).send();};w.prototype.clickHandler=function(event){"use strict";var x=event.getTarget();if(!this.croppingMode&&n.byClass(x,'fbPhotoActionsCrop')){var y=n.byClass(x,'fbPhotoActionsCrop');this.userId=y.getAttribute('data-userid');if(h.hasClass(y,'profileAlbum')){this.showPicInProfileAlbumDialog();}else{var z=!!n.byClass(x,'makeUserProfile');if(z)r.log(null,this.userId,r.action.MAKE_PROFILE);this.enableCropping(z);}event.prevent();}else if(this.croppingMode&&n.byClass(x,'cancelCroppingLink')){r.log(r.step.CANCEL);this.disableCropping(false);event.prevent();}else if(this.croppingMode&&n.byClass(x,'doneCroppingLink')){this.submitCroppedPhoto();event.prevent();}};w.prototype.resetPhoto=function(){"use strict";this.photo=j.find(this.root,'img.spotlight');if(this.photocrop){j.setAttributes(this.photocrop.highlight,{src:this.photo.src});this.photocrop.photo=this.photo;this.photocrop.adjustForResize();}else if(this.wrapper){j.remove(this.wrapper);this.wrapper=null;}};w.prototype.enableCropping=function(x){"use strict";if(this.croppingMode)return;r.log(r.step.CROP);this.croppingMode=true;this.isUserProfilePic=x;this.resetPhoto();this.wrapper=j.create('div');h.addClass(this.wrapper,'stageCropper');j.find(this.root,'.stage').appendChild(this.wrapper);s.set(this.wrapper,'width','100%');s.set(this.wrapper,'height','100%');var y={target:this.wrapper,min_width:p.PIC_NORMAL_FBX_SIZE,min_height:p.PIC_NORMAL_FBX_SIZE};this.photocrop=new o(this.photo,y,this.photoSnowlift.getCurrentImageServerSizeDimensions());h.addClass(this.root,'profilePicCroppingMode');};w.prototype.disableCropping=function(x){"use strict";if(!this.croppingMode)return;this.croppingMode=false;h.removeClass(this.root,'profilePicCroppingMode');var y=null;if(this.photocrop){y=this.photocrop.done(x);this.photocrop=null;}if(!x&&this.wrapper){j.remove(this.wrapper);this.wrapper=null;}delete this.photo;return y;};w.prototype.showPicInProfileAlbumDialog=function(){"use strict";new i().setTitle("Make Profile Picture").setBody("You've used this photo as your profile picture before. Do you want to use it again?").setButtons([i.CONFIRM,i.CANCEL]).setModal(true).setHandler((function(){h.addClass(this.root,'profilePicSavingMode');var x=this.photoSnowlift.getCurrentPhotoInfo();if(!x)return;var y=this.getProfilePicTargetId(x),z=this.isUserProfilePic?'profile':'object',aa={pid:x.pid,owner:x.owner,id:y,type:z,profile_pic_id:y};l.post('/pic_upload.php',aa);}).bind(this)).show();};w.prototype.getProfilePicTargetId=function(x){"use strict";if(this.isUserProfilePic)return this.userId;return x.owner;};w.getInstance=function(x){"use strict";if(!w._instance)w._instance=new w(x);return w._instance;};u(w,{_instance:null});e.exports=w;});