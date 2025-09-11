import { Routes } from '@angular/router';
import { Home } from './home/home';

export const routes: Routes = [
    { path: 'dashboard', component: Home},
    { path: 'analytics', component: Home},
    { path: 'settings', component: Home},
    { path: '', redirectTo: '/dashboard', pathMatch: 'full'},
];
