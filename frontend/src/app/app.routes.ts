import { Routes } from '@angular/router';
import { Home } from './home/home';
import { Livedata } from './livedata/livedata';
import { Analytics } from './analytics/analytics';
import { Settings } from './settings/settings';

export const routes: Routes = [
    { path: 'dashboard', component: Home},
    { path: 'livedata', component: Livedata},
    { path: 'analytics', component: Analytics},
    { path: 'settings', component: Settings},
    { path: '', redirectTo: '/dashboard', pathMatch: 'full'},
];
