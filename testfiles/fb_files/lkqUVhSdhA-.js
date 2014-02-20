/*!CK:704108212!*//*1392692738,178191663*/

if (self.CavalryLogger) { CavalryLogger.start_js(["eGQgv"]); }

__d("PageStars",["Event","Locale","Style","copyProperties"],function(a,b,c,d,e,f,g,h,i,j){var k=5;function l(m,n,o,p,q,r,s,t){this.myRating=q;this.avgRating=p;this.avgStars=n;this.myStars=o;this.margin=t;this.width=r;this.height=s;for(var u=0;u<m.length;u++){var v=m[u],w=u+1;for(var x=0;x<v.length;x++){if(!v[x])continue;g.listen(v[x],'mouseenter',this._hover.bind(this,w));g.listen(v[x],'mouseleave',this._mouseout.bind(this,w));g.listen(v[x],'click',this._click.bind(this,w));}}}j(l.prototype,{onClick:function(m){this.callback=m;},_updateUI:function(){if(this.hovered||this.myRating){this.avgStars&&this._setClip(this.avgStars,0);this.myStars&&this._setClip(this.myStars,this.hovered||this.myRating);}else{this.myStars&&this._setClip(this.myStars,0);this.avgStars&&this._setClip(this.avgStars,this.avgRating||0);}},_setClip:function(m,n){var o=Math.round(n*this.width+Math.floor(n)*this.margin),p=0;if(h.isRTL()){p=Math.round((k-n)*this.width+Math.floor(k-n)*this.margin);o+=p;}var q='rect(0px, '+o+'px, '+this.height+'px, '+p+'px)';i.set(m,'clip',q);},_click:function(m){this.myRating=m;this._updateUI();return this.callback&&this.callback(m);},_hover:function(m){this.hovered=m;this._updateUI();},_mouseout:function(m){if(this.hovered===m){this.hovered=null;this._updateUI();}},setAverageRating:function(m){this.avgRating=m;this._updateUI();},setMyRating:function(m){this.myRating=m;this._updateUI();}});e.exports=l;});
__d("ReviewsController",["Arbiter","Dialog","Event","ReactDOM","React","Image.react","Run","DOM","fbt","ix"],function(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p){var q={},r={},s={},t={},u={},v={registerButton:function(w,x){q[x]=q[x]||[];q[x].push(w);},registerViewerReview:function(w,x){r[x]=r[x]||[];r[x].push(w);},registerComposer:function(w,x){s[x]=s[x]||[];s[x].push(w);},registerToggle:function(w,x,y,z){t[w]=t[w]||[];t[w].push([x,y,z]);},addUserReview:function(w,x,y){this._updateState(true,w,x,y);},deleteUserReview:function(w){this._updateState(false,w);},_updateState:function(w,x,y,z){var aa,ba=r[x]||[];for(aa=0;aa<ba.length;aa++)n.remove(ba[aa]);var ca=t[x]||[];for(aa=0;aa<ca.length;aa++){var da=ca[aa],ea=da[w?1:2];n.setContent(da[0],ea);}var fa=s[x]||[];for(aa=0;aa<fa.length;aa++){var ga=fa[aa];if(w){ga.hide();}else{ga.clear();ga.show();}}var ha=q[x]||[];for(aa=0;aa<ha.length;aa++){var ia=ha[aa],ja=n.find(ia,'.uiButtonText'),ka=n.find(ia,'i'),la,ma;if(w){la=p('/images/icons/buttons/check.png');ma="Reviewed";}else{la=p('/images/icons/buttons/feature.png');ma="Review";}var na=n.create('span'),oa={"padding-right":"5px"};k.renderComponent(j.div(null,ma),ja);k.renderComponent(l({src:la,style:oa}),na);n.replace(ka,na);}if(w){var pa=n.scry(document.body,'.tlPagesTipsList')[0];if(pa)n.prependContent(pa,y);var qa=n.scry(document,'#page_recommendations_browse_list')[0];if(qa)n.prependContent(qa,z);}},registerPhotoUpload:function(w,x){g.subscribe('multi-upload/images-upload-completed/'+x,function(){u[x]=false;});g.subscribe('multi-upload/images-upload-start/'+x,function(){u[x]=true;});i.listen(w,'submit',function(){if(u[x]){new h().setTitle("Photos Are Uploading").setSemiModal(true).setButtons(h.OK).setBody("Please wait until photo is uploaded before posting.").show();return false;}return true;}.bind(this));}};m.onLeave(function(){q={};r={};s={};t={};u={};});e.exports=v;});
__d("PageReviewInlineComposer",["ReviewsController","Event","DOM","CSS","Arbiter"],function(a,b,c,d,e,f,g,h,i,j,k){function l(m,n,o,p,q,r,s){"use strict";var t=i.scry(n,'textarea')[0];if(!s)t&&h.listen(t,'focus',function(){j.addClass(t,'expand');o&&j.show(o);});r.onClick(function(u,v){u.setAttribute('value',v);t.focus();}.bind(null,p));this.$PageReviewInlineComposer0=n;this.$PageReviewInlineComposer1=r;this.$PageReviewInlineComposer2=t;this.$PageReviewInlineComposer3=q;g.registerComposer(this,m);g.registerPhotoUpload(n,q);}l.prototype.hide=function(){"use strict";j.hide(this.$PageReviewInlineComposer0);};l.prototype.show=function(){"use strict";j.show(this.$PageReviewInlineComposer0);};l.prototype.clear=function(){"use strict";this.$PageReviewInlineComposer1.setMyRating(0);this.$PageReviewInlineComposer2.value='';if(this.$PageReviewInlineComposer3!==null)k.inform('multi-upload/composer-reset/'+this.$PageReviewInlineComposer3,this,k.BEHAVIOUR_PERSISTENT);};e.exports=l;});
__d("CovercardArrow",["ContextualDialogArrow","CSS","DOMQuery","Locale","Style","csx","cx"],function(a,b,c,d,e,f,g,h,i,j,k,l,m){var n=-45,o=45,p=12;if(j.isRTL()){n=-n;o=-o;}function q(s){"use strict";this._layer=s;}q.prototype.enable=function(){"use strict";this._layer.enableBehavior(g);var s=this._layer.getContentRoot();this._arrowWrapper=i.scry(s,"._7lh")[0];if(!this._arrowWrapper)return;this._arrowShadow=i.scry(this._arrowWrapper,"._7li")[0];if(!this._arrowShadow)return;var t=null;if(r(this._arrowWrapper))t=this._renderArrowWithRotate.bind(this);if(!t)return;if(j.isRTL())h.addClass(s,"_7lf");this._subscription=this._layer.subscribe('reposition',function(u,v){var w=v.getPosition()=='below';h.conditionClass(s,"_53ih",w);w&&t(v);});};q.prototype.disable=function(){"use strict";this._subscription&&this._subscription.unsubscribe();this._subscription=null;};q.prototype._calculateArrowOffset=function(s){"use strict";var t=g.getOffsetPercent(s),u=g.getOffset(s,t,this._layer),v=k.get(this._layer.getContentRoot(),'width');return parseInt(v,10)*(t/100)+u;};q.prototype._renderArrowWithRotate=function(s){"use strict";var t=i.scry(this._arrowWrapper,"._7lj")[0];if(!t)return;var u=r(this._arrowWrapper);if(!u)return;var v=this._calculateArrowOffset(s),w=p+v;if(j.isRTL())w=-w;this._arrowWrapper.style[u]='translate('+w+'px, -'+p+'px) '+'rotate('+o+'deg)';t.style[u]='rotate('+n+'deg) '+'translate('+(-w)+'px, 0px)';};function r(s){if(!s)s=document.body;var t=['transform','WebkitTransform','msTransform','MozTransform','OTransform'],u;while(u=t.shift())if(s.style[u]!==undefined)return u;return null;}e.exports=q;});