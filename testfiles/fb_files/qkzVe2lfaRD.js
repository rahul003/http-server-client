/*!CK:1787319255!*//*1392701115,178191169*/

if (self.CavalryLogger) { CavalryLogger.start_js(["Myzzo"]); }

__d("TypeaheadMetricsX",["Event","TypeaheadMetricReporter","SubscriptionsHandler","copyProperties","emptyFunction","getActiveElement"],function(a,b,c,d,e,f,g,h,i,j,k,l){function m(n){"use strict";this.extraData={};j(this,n);}m.prototype.init=function(n){"use strict";this.init=k;this.core=n.getCore();this.view=n.getView();this.data=n.getData();this._startSubscriptions=new i();this._endSubscriptions=new i();this._initReporter();this._initEvents();};m.prototype.bindSessionStart=function(n,event,o){"use strict";o&&this._startSubscriptions.release();this._startSubscriptions.addSubscriptions(n.subscribe(event,this.reporter.sessionStart.bind(this.reporter)));};m.prototype.bindSessionEnd=function(n,event,o){"use strict";o&&this._endSubscriptions.release();this._endSubscriptions.addSubscriptions(n.subscribe(event,this.reporter.sessionEnd.bind(this.reporter)));};m.prototype._initReporter=function(){"use strict";this.reporter=new h(this.extraData||{});this.reporter.addListener('reset',function(n){this.data.setQueryData({sid:n.sid});}.bind(this));};m.prototype._initEvents=function(){"use strict";this.bindSessionStart(this.core,'focus');this.bindSessionEnd(this.core,'blur');this.view.subscribe('select',function(n,o){var p=o.selected;this.reporter.reportSelect(p.uid,p.type,o.index,o.clicked,o.query);this.reporter.sessionEnd();}.bind(this));this.view.subscribe('render',function(n,o){this.reporter.reportResults(o.map(function(p){return p.uid;}));}.bind(this));this.data.subscribe('beforeQuery',function(n,o){if(o.value)this.reporter.reportQueryBegin(o.value);}.bind(this));this.data.subscribe('beforeFetch',function(n,o){if(o.fetch_context.bootstrap){this.reporter.reportBootstrapBegin();}else o.fetch_context.queryBegin=Date.now();}.bind(this));this.data.subscribe('fetchComplete',function(n,o){if(o.fetch_context.bootstrap){var p=o.response.payload.entries.map(function(q){return q.type;});this.reporter.reportBootstrapComplete(p);}else this.reporter.reportQueryComplete(Date.now()-o.fetch_context.queryBegin,o.response.payload.filtered_count);}.bind(this));this.data.subscribe('dirty',function(n,o){this.reporter.reportBootstrapDirty();}.bind(this));};m.register=function(n,o,p){"use strict";if(l()===n){o.init(p);}else var q=g.listen(n,'focus',function(){o.init(p);q.remove();});};e.exports=m;});