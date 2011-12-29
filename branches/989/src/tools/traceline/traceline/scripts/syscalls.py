# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

syscalls = {
0: "ntdll.dll!NtAcceptConnectPort",
1: "ntdll.dll!NtAccessCheck",
2: "ntdll.dll!ZwAccessCheckAndAuditAlarm",
3: "ntdll.dll!NtAccessCheckByType",
4: "ntdll.dll!NtAccessCheckByTypeAndAuditAlarm",
5: "ntdll.dll!NtAccessCheckByTypeResultList",
6: "ntdll.dll!NtAccessCheckByTypeResultListAndAuditAlarm",
7: "ntdll.dll!ZwAccessCheckByTypeResultListAndAuditAlarmByHandle",
8: "ntdll.dll!NtAddAtom",
9: "ntdll.dll!ZwAddBootEntry",
10: "ntdll.dll!ZwAdjustGroupsToken",
11: "ntdll.dll!ZwAdjustPrivilegesToken",
12: "ntdll.dll!NtAlertResumeThread",
13: "ntdll.dll!NtAlertThread",
14: "ntdll.dll!ZwAllocateLocallyUniqueId",
15: "ntdll.dll!NtAllocateUserPhysicalPages",
16: "ntdll.dll!NtAllocateUuids",
17: "ntdll.dll!NtAllocateVirtualMemory",
18: "ntdll.dll!ZwAreMappedFilesTheSame",
19: "ntdll.dll!ZwAssignProcessToJobObject",
20: "ntdll.dll!ZwCallbackReturn",
21: "ntdll.dll!NtCancelDeviceWakeupRequest",
22: "ntdll.dll!ZwCancelIoFile",
23: "ntdll.dll!ZwCancelTimer",
24: "ntdll.dll!NtClearEvent",
25: "ntdll.dll!NtClose",
26: "ntdll.dll!ZwCloseObjectAuditAlarm",
27: "ntdll.dll!NtCompactKeys",
28: "ntdll.dll!ZwCompareTokens",
29: "ntdll.dll!NtCompleteConnectPort",
30: "ntdll.dll!ZwCompressKey",
31: "ntdll.dll!NtConnectPort",
32: "ntdll.dll!ZwContinue",
33: "ntdll.dll!ZwCreateDebugObject",
34: "ntdll.dll!ZwCreateDirectoryObject",
35: "ntdll.dll!NtCreateEvent",
36: "ntdll.dll!NtCreateEventPair",
37: "ntdll.dll!NtCreateFile",
38: "ntdll.dll!NtCreateIoCompletion",
39: "ntdll.dll!ZwCreateJobObject",
40: "ntdll.dll!NtCreateJobSet",
41: "ntdll.dll!ZwCreateKey",
42: "ntdll.dll!ZwCreateMailslotFile",
43: "ntdll.dll!ZwCreateMutant",
44: "ntdll.dll!ZwCreateNamedPipeFile",
45: "ntdll.dll!NtCreatePagingFile",
46: "ntdll.dll!ZwCreatePort",
47: "ntdll.dll!ZwCreateProcess",
48: "ntdll.dll!ZwCreateProcessEx",
49: "ntdll.dll!ZwCreateProfile",
50: "ntdll.dll!NtCreateSection",
51: "ntdll.dll!NtCreateSemaphore",
52: "ntdll.dll!ZwCreateSymbolicLinkObject",
53: "ntdll.dll!NtCreateThread",
54: "ntdll.dll!ZwCreateTimer",
55: "ntdll.dll!NtCreateToken",
56: "ntdll.dll!ZwCreateWaitablePort",
57: "ntdll.dll!NtDebugActiveProcess",
58: "ntdll.dll!ZwDebugContinue",
59: "ntdll.dll!ZwDelayExecution",
60: "ntdll.dll!ZwDeleteAtom",
61: "ntdll.dll!NtDeleteBootEntry",
62: "ntdll.dll!NtDeleteFile",
63: "ntdll.dll!ZwDeleteKey",
64: "ntdll.dll!NtDeleteObjectAuditAlarm",
65: "ntdll.dll!NtDeleteValueKey",
66: "ntdll.dll!ZwDeviceIoControlFile",
67: "ntdll.dll!NtDisplayString",
68: "ntdll.dll!ZwDuplicateObject",
69: "ntdll.dll!NtDuplicateToken",
70: "ntdll.dll!ZwEnumerateBootEntries",
71: "ntdll.dll!ZwEnumerateKey",
72: "ntdll.dll!ZwEnumerateSystemEnvironmentValuesEx",
73: "ntdll.dll!NtEnumerateValueKey",
74: "ntdll.dll!ZwExtendSection",
75: "ntdll.dll!NtFilterToken",
76: "ntdll.dll!NtFindAtom",
77: "ntdll.dll!ZwFlushBuffersFile",
78: "ntdll.dll!ZwFlushInstructionCache",
79: "ntdll.dll!NtFlushKey",
80: "ntdll.dll!ZwFlushVirtualMemory",
81: "ntdll.dll!NtFlushWriteBuffer",
82: "ntdll.dll!NtFreeUserPhysicalPages",
83: "ntdll.dll!NtFreeVirtualMemory",
84: "ntdll.dll!NtFsControlFile",
85: "ntdll.dll!NtGetContextThread",
86: "ntdll.dll!NtGetDevicePowerState",
87: "ntdll.dll!ZwGetPlugPlayEvent",
88: "ntdll.dll!NtGetWriteWatch",
89: "ntdll.dll!NtImpersonateAnonymousToken",
90: "ntdll.dll!ZwImpersonateClientOfPort",
91: "ntdll.dll!ZwImpersonateThread",
92: "ntdll.dll!ZwInitializeRegistry",
93: "ntdll.dll!NtInitiatePowerAction",
94: "ntdll.dll!ZwIsProcessInJob",
95: "ntdll.dll!NtIsSystemResumeAutomatic",
96: "ntdll.dll!ZwListenPort",
97: "ntdll.dll!NtLoadDriver",
98: "ntdll.dll!NtLoadKey",
99: "ntdll.dll!NtLoadKey2",
100: "ntdll.dll!NtLockFile",
101: "ntdll.dll!ZwLockProductActivationKeys",
102: "ntdll.dll!NtLockRegistryKey",
103: "ntdll.dll!ZwLockVirtualMemory",
104: "ntdll.dll!ZwMakePermanentObject",
105: "ntdll.dll!NtMakeTemporaryObject",
106: "ntdll.dll!NtMapUserPhysicalPages",
107: "ntdll.dll!ZwMapUserPhysicalPagesScatter",
108: "ntdll.dll!ZwMapViewOfSection",
109: "ntdll.dll!NtModifyBootEntry",
110: "ntdll.dll!NtNotifyChangeDirectoryFile",
111: "ntdll.dll!NtNotifyChangeKey",
112: "ntdll.dll!NtNotifyChangeMultipleKeys",
113: "ntdll.dll!ZwOpenDirectoryObject",
114: "ntdll.dll!NtOpenEvent",
115: "ntdll.dll!NtOpenEventPair",
116: "ntdll.dll!NtOpenFile",
117: "ntdll.dll!ZwOpenIoCompletion",
118: "ntdll.dll!ZwOpenJobObject",
119: "ntdll.dll!ZwOpenKey",
120: "ntdll.dll!NtOpenMutant",
121: "ntdll.dll!ZwOpenObjectAuditAlarm",
122: "ntdll.dll!ZwOpenProcess",
123: "ntdll.dll!ZwOpenProcessToken",
124: "ntdll.dll!ZwOpenProcessTokenEx",
125: "ntdll.dll!NtOpenSection",
126: "ntdll.dll!NtOpenSemaphore",
127: "ntdll.dll!NtOpenSymbolicLinkObject",
128: "ntdll.dll!ZwOpenThread",
129: "ntdll.dll!NtOpenThreadToken",
130: "ntdll.dll!NtOpenThreadTokenEx",
131: "ntdll.dll!ZwOpenTimer",
132: "ntdll.dll!NtPlugPlayControl",
133: "ntdll.dll!ZwPowerInformation",
134: "ntdll.dll!ZwPrivilegeCheck",
135: "ntdll.dll!ZwPrivilegeObjectAuditAlarm",
136: "ntdll.dll!NtPrivilegedServiceAuditAlarm",
137: "ntdll.dll!ZwProtectVirtualMemory",
138: "ntdll.dll!ZwPulseEvent",
139: "ntdll.dll!ZwQueryAttributesFile",
140: "ntdll.dll!ZwQueryBootEntryOrder",
141: "ntdll.dll!ZwQueryBootOptions",
142: "ntdll.dll!NtQueryDebugFilterState",
143: "ntdll.dll!NtQueryDefaultLocale",
144: "ntdll.dll!ZwQueryDefaultUILanguage",
145: "ntdll.dll!ZwQueryDirectoryFile",
146: "ntdll.dll!ZwQueryDirectoryObject",
147: "ntdll.dll!ZwQueryEaFile",
148: "ntdll.dll!NtQueryEvent",
149: "ntdll.dll!ZwQueryFullAttributesFile",
150: "ntdll.dll!NtQueryInformationAtom",
151: "ntdll.dll!ZwQueryInformationFile",
152: "ntdll.dll!ZwQueryInformationJobObject",
153: "ntdll.dll!ZwQueryInformationPort",
154: "ntdll.dll!ZwQueryInformationProcess",
155: "ntdll.dll!NtQueryInformationThread",
156: "ntdll.dll!ZwQueryInformationToken",
157: "ntdll.dll!NtQueryInstallUILanguage",
158: "ntdll.dll!NtQueryIntervalProfile",
159: "ntdll.dll!NtQueryIoCompletion",
160: "ntdll.dll!ZwQueryKey",
161: "ntdll.dll!NtQueryMultipleValueKey",
162: "ntdll.dll!NtQueryMutant",
163: "ntdll.dll!NtQueryObject",
164: "ntdll.dll!NtQueryOpenSubKeys",
165: "ntdll.dll!NtQueryPerformanceCounter",
166: "ntdll.dll!ZwQueryQuotaInformationFile",
167: "ntdll.dll!ZwQuerySection",
168: "ntdll.dll!NtQuerySecurityObject",
169: "ntdll.dll!ZwQuerySemaphore",
170: "ntdll.dll!ZwQuerySymbolicLinkObject",
171: "ntdll.dll!ZwQuerySystemEnvironmentValue",
172: "ntdll.dll!ZwQuerySystemEnvironmentValueEx",
173: "ntdll.dll!NtQuerySystemInformation",
174: "ntdll.dll!NtQuerySystemTime",
175: "ntdll.dll!ZwQueryTimer",
176: "ntdll.dll!NtQueryTimerResolution",
177: "ntdll.dll!ZwQueryValueKey",
178: "ntdll.dll!NtQueryVirtualMemory",
179: "ntdll.dll!NtQueryVolumeInformationFile",
180: "ntdll.dll!NtQueueApcThread",
181: "ntdll.dll!ZwRaiseException",
182: "ntdll.dll!ZwRaiseHardError",
183: "ntdll.dll!NtReadFile",
184: "ntdll.dll!NtReadFileScatter",
185: "ntdll.dll!ZwReadRequestData",
186: "ntdll.dll!NtReadVirtualMemory",
187: "ntdll.dll!ZwRegisterThreadTerminatePort",
188: "ntdll.dll!ZwReleaseMutant",
189: "ntdll.dll!NtReleaseSemaphore",
190: "ntdll.dll!ZwRemoveIoCompletion",
191: "ntdll.dll!ZwRemoveProcessDebug",
192: "ntdll.dll!ZwRenameKey",
193: "ntdll.dll!ZwReplaceKey",
194: "ntdll.dll!ZwReplyPort",
195: "ntdll.dll!NtReplyWaitReceivePort",
196: "ntdll.dll!NtReplyWaitReceivePortEx",
197: "ntdll.dll!NtReplyWaitReplyPort",
198: "ntdll.dll!ZwRequestDeviceWakeup",
199: "ntdll.dll!ZwRequestPort",
200: "ntdll.dll!NtRequestWaitReplyPort",
201: "ntdll.dll!ZwRequestWakeupLatency",
202: "ntdll.dll!NtResetEvent",
203: "ntdll.dll!ZwResetWriteWatch",
204: "ntdll.dll!NtRestoreKey",
205: "ntdll.dll!ZwResumeProcess",
206: "ntdll.dll!ZwResumeThread",
207: "ntdll.dll!NtSaveKey",
208: "ntdll.dll!NtSaveKeyEx",
209: "ntdll.dll!NtSaveMergedKeys",
210: "ntdll.dll!NtSecureConnectPort",
211: "ntdll.dll!ZwSetBootEntryOrder",
212: "ntdll.dll!ZwSetBootOptions",
213: "ntdll.dll!ZwSetContextThread",
214: "ntdll.dll!NtSetDebugFilterState",
215: "ntdll.dll!NtSetDefaultHardErrorPort",
216: "ntdll.dll!NtSetDefaultLocale",
217: "ntdll.dll!ZwSetDefaultUILanguage",
218: "ntdll.dll!ZwSetEaFile",
219: "ntdll.dll!NtSetEvent",
220: "ntdll.dll!NtSetEventBoostPriority",
221: "ntdll.dll!NtSetHighEventPair",
222: "ntdll.dll!NtSetHighWaitLowEventPair",
223: "ntdll.dll!ZwSetInformationDebugObject",
224: "ntdll.dll!ZwSetInformationFile",
225: "ntdll.dll!ZwSetInformationJobObject",
226: "ntdll.dll!ZwSetInformationKey",
227: "ntdll.dll!ZwSetInformationObject",
228: "ntdll.dll!ZwSetInformationProcess",
229: "ntdll.dll!ZwSetInformationThread",
230: "ntdll.dll!ZwSetInformationToken",
231: "ntdll.dll!NtSetIntervalProfile",
232: "ntdll.dll!NtSetIoCompletion",
233: "ntdll.dll!ZwSetLdtEntries",
234: "ntdll.dll!ZwSetLowEventPair",
235: "ntdll.dll!ZwSetLowWaitHighEventPair",
236: "ntdll.dll!ZwSetQuotaInformationFile",
237: "ntdll.dll!NtSetSecurityObject",
238: "ntdll.dll!ZwSetSystemEnvironmentValue",
239: "ntdll.dll!ZwSetSystemEnvironmentValueEx",
240: "ntdll.dll!ZwSetSystemInformation",
241: "ntdll.dll!ZwSetSystemPowerState",
242: "ntdll.dll!ZwSetSystemTime",
243: "ntdll.dll!ZwSetThreadExecutionState",
244: "ntdll.dll!ZwSetTimer",
245: "ntdll.dll!NtSetTimerResolution",
246: "ntdll.dll!ZwSetUuidSeed",
247: "ntdll.dll!ZwSetValueKey",
248: "ntdll.dll!NtSetVolumeInformationFile",
249: "ntdll.dll!ZwShutdownSystem",
250: "ntdll.dll!ZwSignalAndWaitForSingleObject",
251: "ntdll.dll!NtStartProfile",
252: "ntdll.dll!ZwStopProfile",
253: "ntdll.dll!ZwSuspendProcess",
254: "ntdll.dll!ZwSuspendThread",
255: "ntdll.dll!NtSystemDebugControl",
256: "ntdll.dll!ZwTerminateJobObject",
257: "ntdll.dll!ZwTerminateProcess",
258: "ntdll.dll!ZwTerminateThread",
259: "ntdll.dll!NtTestAlert",
260: "ntdll.dll!NtTraceEvent",
261: "ntdll.dll!NtTranslateFilePath",
262: "ntdll.dll!ZwUnloadDriver",
263: "ntdll.dll!NtUnloadKey",
264: "ntdll.dll!ZwUnloadKeyEx",
265: "ntdll.dll!ZwUnlockFile",
266: "ntdll.dll!NtUnlockVirtualMemory",
267: "ntdll.dll!NtUnmapViewOfSection",
268: "ntdll.dll!NtVdmControl",
269: "ntdll.dll!NtWaitForDebugEvent",
270: "ntdll.dll!NtWaitForMultipleObjects",
271: "ntdll.dll!ZwWaitForSingleObject",
272: "ntdll.dll!ZwWaitHighEventPair",
273: "ntdll.dll!NtWaitLowEventPair",
274: "ntdll.dll!NtWriteFile",
275: "ntdll.dll!NtWriteFileGather",
276: "ntdll.dll!NtWriteRequestData",
277: "ntdll.dll!NtWriteVirtualMemory",
278: "ntdll.dll!ZwYieldExecution",
279: "ntdll.dll!ZwCreateKeyedEvent",
280: "ntdll.dll!NtOpenKeyedEvent",
281: "ntdll.dll!NtReleaseKeyedEvent",
282: "ntdll.dll!NtWaitForKeyedEvent",
283: "ntdll.dll!ZwQueryPortInformationProcess",
4096: "gdi32.dll!NtGdiAbortDoc",
4097: "gdi32.dll!NtGdiAbortPath",
4098: "gdi32.dll!NtGdiAddFontResourceW",
4099: "gdi32.dll!NtGdiAddRemoteFontToDC",
4100: "gdi32.dll!NtGdiAddFontMemResourceEx",
4101: "gdi32.dll!NtGdiRemoveMergeFont",
4102: "gdi32.dll!NtGdiAddRemoteMMInstanceToDC",
4103: "gdi32.dll!NtGdiAlphaBlend",
4104: "gdi32.dll!NtGdiAngleArc",
4105: "gdi32.dll!NtGdiAnyLinkedFonts",
4106: "gdi32.dll!NtGdiFontIsLinked",
4107: "gdi32.dll!NtGdiArcInternal",
4108: "gdi32.dll!NtGdiBeginPath",
4109: "gdi32.dll!NtGdiBitBlt",
4110: "gdi32.dll!NtGdiCancelDC",
4111: "gdi32.dll!NtGdiCheckBitmapBits",
4112: "gdi32.dll!NtGdiCloseFigure",
4113: "gdi32.dll!NtGdiClearBitmapAttributes",
4114: "gdi32.dll!NtGdiClearBrushAttributes",
4115: "gdi32.dll!NtGdiColorCorrectPalette",
4116: "gdi32.dll!NtGdiCombineRgn",
4117: "gdi32.dll!CombineTransform",
4118: "gdi32.dll!NtGdiComputeXformCoefficients",
4119: "gdi32.dll!NtGdiConsoleTextOut",
4120: "gdi32.dll!NtGdiConvertMetafileRect",
4121: "gdi32.dll!NtGdiCreateBitmap",
4122: "gdi32.dll!NtGdiCreateClientObj",
4123: "gdi32.dll!NtGdiCreateColorSpace",
4124: "gdi32.dll!NtGdiCreateColorTransform",
4125: "gdi32.dll!NtGdiCreateCompatibleBitmap",
4126: "gdi32.dll!NtGdiCreateCompatibleDC",
4127: "gdi32.dll!NtGdiCreateDIBBrush",
4128: "gdi32.dll!NtGdiCreateDIBitmapInternal",
4129: "gdi32.dll!NtGdiCreateDIBSection",
4130: "gdi32.dll!NtGdiCreateEllipticRgn",
4131: "gdi32.dll!NtGdiCreateHalftonePalette",
4132: "gdi32.dll!NtGdiCreateHatchBrushInternal",
4133: "gdi32.dll!NtGdiCreateMetafileDC",
4134: "gdi32.dll!NtGdiCreatePaletteInternal",
4135: "gdi32.dll!NtGdiCreatePatternBrushInternal",
4136: "gdi32.dll!CreatePen",
4137: "gdi32.dll!NtGdiCreateRectRgn",
4138: "gdi32.dll!CreateRoundRectRgn",
4139: "gdi32.dll!NtGdiCreateServerMetaFile",
4140: "gdi32.dll!NtGdiCreateSolidBrush",
4141: "gdi32.dll!NtGdiD3dContextCreate",
4142: "gdi32.dll!NtGdiD3dContextDestroy",
4143: "gdi32.dll!NtGdiD3dContextDestroyAll",
4144: "gdi32.dll!NtGdiD3dValidateTextureStageState",
4145: "gdi32.dll!NtGdiD3dDrawPrimitives2",
4146: "gdi32.dll!NtGdiDdGetDriverState",
4147: "gdi32.dll!NtGdiDdAddAttachedSurface",
4148: "gdi32.dll!NtGdiDdAlphaBlt",
4149: "gdi32.dll!NtGdiDdAttachSurface",
4150: "gdi32.dll!NtGdiDdBeginMoCompFrame",
4151: "gdi32.dll!NtGdiDdBlt",
4152: "gdi32.dll!NtGdiDdCanCreateSurface",
4153: "gdi32.dll!NtGdiDdCanCreateD3DBuffer",
4154: "gdi32.dll!NtGdiDdColorControl",
4155: "gdi32.dll!NtGdiDdCreateDirectDrawObject",
4156: "gdi32.dll!NtGdiDdCreateSurface",
4157: "gdi32.dll!NtGdiDdCreateD3DBuffer",
4158: "gdi32.dll!NtGdiDdCreateMoComp",
4159: "gdi32.dll!NtGdiDdCreateSurfaceObject",
4160: "gdi32.dll!NtGdiDdDeleteDirectDrawObject",
4161: "gdi32.dll!NtGdiDdDeleteSurfaceObject",
4162: "gdi32.dll!NtGdiDdDestroyMoComp",
4163: "gdi32.dll!NtGdiDdDestroySurface",
4164: "gdi32.dll!NtGdiDdDestroyD3DBuffer",
4165: "gdi32.dll!NtGdiDdEndMoCompFrame",
4166: "gdi32.dll!NtGdiDdFlip",
4167: "gdi32.dll!NtGdiDdFlipToGDISurface",
4168: "gdi32.dll!NtGdiDdGetAvailDriverMemory",
4169: "gdi32.dll!NtGdiDdGetBltStatus",
4170: "gdi32.dll!NtGdiDdGetDC",
4171: "gdi32.dll!NtGdiDdGetDriverInfo",
4172: "gdi32.dll!NtGdiDdGetDxHandle",
4173: "gdi32.dll!NtGdiDdGetFlipStatus",
4174: "gdi32.dll!NtGdiDdGetInternalMoCompInfo",
4175: "gdi32.dll!NtGdiDdGetMoCompBuffInfo",
4176: "gdi32.dll!NtGdiDdGetMoCompGuids",
4177: "gdi32.dll!NtGdiDdGetMoCompFormats",
4178: "gdi32.dll!NtGdiDdGetScanLine",
4179: "gdi32.dll!NtGdiDdLock",
4180: "gdi32.dll!NtGdiDdLockD3D",
4181: "gdi32.dll!NtGdiDdQueryDirectDrawObject",
4182: "gdi32.dll!NtGdiDdQueryMoCompStatus",
4183: "gdi32.dll!NtGdiDdReenableDirectDrawObject",
4184: "gdi32.dll!NtGdiDdReleaseDC",
4185: "gdi32.dll!NtGdiDdRenderMoComp",
4186: "gdi32.dll!NtGdiDdResetVisrgn",
4187: "gdi32.dll!NtGdiDdSetColorKey",
4188: "gdi32.dll!NtGdiDdSetExclusiveMode",
4189: "gdi32.dll!NtGdiDdSetGammaRamp",
4190: "gdi32.dll!NtGdiDdCreateSurfaceEx",
4191: "gdi32.dll!NtGdiDdSetOverlayPosition",
4192: "gdi32.dll!NtGdiDdUnattachSurface",
4193: "gdi32.dll!NtGdiDdUnlock",
4194: "gdi32.dll!NtGdiDdUnlockD3D",
4195: "gdi32.dll!NtGdiDdUpdateOverlay",
4196: "gdi32.dll!NtGdiDdWaitForVerticalBlank",
4197: "gdi32.dll!NtGdiDvpCanCreateVideoPort",
4198: "gdi32.dll!NtGdiDvpColorControl",
4199: "gdi32.dll!NtGdiDvpCreateVideoPort",
4200: "gdi32.dll!NtGdiDvpDestroyVideoPort",
4201: "gdi32.dll!NtGdiDvpFlipVideoPort",
4202: "gdi32.dll!NtGdiDvpGetVideoPortBandwidth",
4203: "gdi32.dll!NtGdiDvpGetVideoPortField",
4204: "gdi32.dll!NtGdiDvpGetVideoPortFlipStatus",
4205: "gdi32.dll!NtGdiDvpGetVideoPortInputFormats",
4206: "gdi32.dll!NtGdiDvpGetVideoPortLine",
4207: "gdi32.dll!NtGdiDvpGetVideoPortOutputFormats",
4208: "gdi32.dll!NtGdiDvpGetVideoPortConnectInfo",
4209: "gdi32.dll!NtGdiDvpGetVideoSignalStatus",
4210: "gdi32.dll!NtGdiDvpUpdateVideoPort",
4211: "gdi32.dll!NtGdiDvpWaitForVideoPortSync",
4212: "gdi32.dll!NtGdiDvpAcquireNotification",
4213: "gdi32.dll!NtGdiDvpReleaseNotification",
4214: "gdi32.dll!NtGdiDxgGenericThunk",
4215: "gdi32.dll!NtGdiDeleteClientObj",
4216: "gdi32.dll!NtGdiDeleteColorSpace",
4217: "gdi32.dll!NtGdiDeleteColorTransform",
4218: "gdi32.dll!DeleteObject",
4219: "gdi32.dll!NtGdiDescribePixelFormat",
4220: "gdi32.dll!NtGdiGetPerBandInfo",
4221: "gdi32.dll!NtGdiDoBanding",
4222: "gdi32.dll!NtGdiDoPalette",
4223: "gdi32.dll!NtGdiDrawEscape",
4224: "gdi32.dll!NtGdiEllipse",
4225: "gdi32.dll!NtGdiEnableEudc",
4226: "gdi32.dll!NtGdiEndDoc",
4227: "gdi32.dll!NtGdiEndPage",
4228: "gdi32.dll!NtGdiEndPath",
4229: "gdi32.dll!NtGdiEnumFontChunk",
4230: "gdi32.dll!NtGdiEnumFontClose",
4231: "gdi32.dll!NtGdiEnumFontOpen",
4232: "gdi32.dll!NtGdiEnumObjects",
4233: "gdi32.dll!NtGdiEqualRgn",
4234: "gdi32.dll!NtGdiEudcLoadUnloadLink",
4235: "gdi32.dll!NtGdiExcludeClipRect",
4236: "gdi32.dll!NtGdiExtCreatePen",
4237: "gdi32.dll!NtGdiExtCreateRegion",
4238: "gdi32.dll!NtGdiExtEscape",
4239: "gdi32.dll!NtGdiExtFloodFill",
4240: "gdi32.dll!NtGdiExtGetObjectW",
4241: "gdi32.dll!ExtSelectClipRgn",
4242: "gdi32.dll!NtGdiExtTextOutW",
4243: "gdi32.dll!NtGdiFillPath",
4244: "gdi32.dll!NtGdiFillRgn",
4245: "gdi32.dll!NtGdiFlattenPath",
4247: "gdi32.dll!NtGdiFlush",
4248: "gdi32.dll!NtGdiForceUFIMapping",
4249: "gdi32.dll!NtGdiFrameRgn",
4250: "gdi32.dll!NtGdiFullscreenControl",
4251: "gdi32.dll!NtGdiGetAndSetDCDword",
4252: "gdi32.dll!GetClipBox",
4253: "gdi32.dll!GetBitmapBits",
4254: "gdi32.dll!NtGdiGetBitmapDimension",
4255: "gdi32.dll!NtGdiGetBoundsRect",
4256: "gdi32.dll!NtGdiGetCharABCWidthsW",
4257: "gdi32.dll!NtGdiGetCharacterPlacementW",
4258: "gdi32.dll!NtGdiGetCharSet",
4259: "gdi32.dll!NtGdiGetCharWidthW",
4260: "gdi32.dll!NtGdiGetCharWidthInfo",
4261: "gdi32.dll!NtGdiGetColorAdjustment",
4263: "gdi32.dll!NtGdiGetDCDword",
4264: "gdi32.dll!NtGdiGetDCforBitmap",
4265: "gdi32.dll!NtGdiGetDCObject",
4266: "gdi32.dll!NtGdiGetDCPoint",
4267: "gdi32.dll!NtGdiGetDeviceCaps",
4268: "gdi32.dll!NtGdiGetDeviceGammaRamp",
4269: "gdi32.dll!NtGdiGetDeviceCapsAll",
4270: "gdi32.dll!NtGdiGetDIBitsInternal",
4271: "gdi32.dll!NtGdiGetETM",
4272: "gdi32.dll!NtGdiGetEudcTimeStampEx",
4273: "gdi32.dll!GetFontData",
4274: "gdi32.dll!NtGdiGetFontResourceInfoInternalW",
4275: "gdi32.dll!NtGdiGetGlyphIndicesW",
4276: "gdi32.dll!NtGdiGetGlyphIndicesWInternal",
4277: "gdi32.dll!NtGdiGetGlyphOutline",
4278: "gdi32.dll!NtGdiGetKerningPairs",
4279: "gdi32.dll!NtGdiGetLinkedUFIs",
4280: "gdi32.dll!GetMiterLimit",
4281: "gdi32.dll!NtGdiGetMonitorID",
4282: "gdi32.dll!GetNearestColor",
4283: "gdi32.dll!NtGdiGetNearestPaletteIndex",
4284: "gdi32.dll!NtGdiGetObjectBitmapHandle",
4285: "gdi32.dll!NtGdiGetOutlineTextMetricsInternalW",
4286: "gdi32.dll!NtGdiGetPath",
4287: "gdi32.dll!NtGdiGetPixel",
4288: "gdi32.dll!NtGdiGetRandomRgn",
4289: "gdi32.dll!GetRasterizerCaps",
4290: "gdi32.dll!NtGdiGetRealizationInfo",
4291: "gdi32.dll!GetRegionData",
4292: "gdi32.dll!NtGdiGetRgnBox",
4293: "gdi32.dll!NtGdiGetServerMetaFileBits",
4294: "gdi32.dll!NtGdiGetSpoolMessage",
4296: "gdi32.dll!NtGdiGetStockObject",
4297: "gdi32.dll!NtGdiGetStringBitmapW",
4298: "gdi32.dll!GetSystemPaletteUse",
4299: "gdi32.dll!NtGdiGetTextCharsetInfo",
4300: "gdi32.dll!NtGdiGetTextExtent",
4301: "gdi32.dll!NtGdiGetTextExtentExW",
4302: "gdi32.dll!NtGdiGetTextFaceW",
4303: "gdi32.dll!NtGdiGetTextMetricsW",
4304: "gdi32.dll!NtGdiGetTransform",
4305: "gdi32.dll!NtGdiGetUFI",
4306: "gdi32.dll!NtGdiGetEmbUFI",
4307: "gdi32.dll!NtGdiGetUFIPathname",
4308: "gdi32.dll!NtGdiGetEmbedFonts",
4309: "gdi32.dll!NtGdiChangeGhostFont",
4310: "gdi32.dll!NtGdiAddEmbFontToDC",
4311: "gdi32.dll!NtGdiGetFontUnicodeRanges",
4312: "gdi32.dll!NtGdiGetWidthTable",
4313: "gdi32.dll!NtGdiGradientFill",
4314: "gdi32.dll!NtGdiHfontCreate",
4315: "gdi32.dll!NtGdiIcmBrushInfo",
4316: "gdi32.dll!NtGdiInit",
4317: "gdi32.dll!NtGdiInitSpool",
4318: "gdi32.dll!NtGdiIntersectClipRect",
4319: "gdi32.dll!NtGdiInvertRgn",
4320: "gdi32.dll!NtGdiLineTo",
4321: "gdi32.dll!NtGdiMakeFontDir",
4322: "gdi32.dll!NtGdiMakeInfoDC",
4323: "gdi32.dll!NtGdiMaskBlt",
4324: "gdi32.dll!NtGdiModifyWorldTransform",
4325: "gdi32.dll!NtGdiMonoBitmap",
4327: "gdi32.dll!NtGdiOffsetClipRgn",
4328: "gdi32.dll!NtGdiOffsetRgn",
4329: "gdi32.dll!NtGdiOpenDCW",
4330: "gdi32.dll!NtGdiPatBlt",
4331: "gdi32.dll!NtGdiPolyPatBlt",
4332: "gdi32.dll!NtGdiPathToRegion",
4333: "gdi32.dll!NtGdiPlgBlt",
4334: "gdi32.dll!NtGdiPolyDraw",
4335: "gdi32.dll!NtGdiPolyPolyDraw",
4336: "gdi32.dll!NtGdiPolyTextOutW",
4337: "gdi32.dll!NtGdiPtInRegion",
4338: "gdi32.dll!NtGdiPtVisible",
4339: "gdi32.dll!NtGdiQueryFonts",
4340: "gdi32.dll!NtGdiQueryFontAssocInfo",
4341: "gdi32.dll!NtGdiRectangle",
4342: "gdi32.dll!NtGdiRectInRegion",
4343: "gdi32.dll!RectVisible",
4344: "gdi32.dll!NtGdiRemoveFontResourceW",
4345: "gdi32.dll!NtGdiRemoveFontMemResourceEx",
4346: "gdi32.dll!NtGdiResetDC",
4347: "gdi32.dll!NtGdiResizePalette",
4348: "gdi32.dll!NtGdiRestoreDC",
4349: "gdi32.dll!NtGdiRoundRect",
4350: "gdi32.dll!NtGdiSaveDC",
4351: "gdi32.dll!NtGdiScaleViewportExtEx",
4352: "gdi32.dll!NtGdiScaleWindowExtEx",
4353: "gdi32.dll!NtGdiSelectBitmap",
4355: "gdi32.dll!NtGdiSelectClipPath",
4356: "gdi32.dll!NtGdiSelectFont",
4357: "gdi32.dll!NtGdiSelectPen",
4358: "gdi32.dll!NtGdiSetBitmapAttributes",
4359: "gdi32.dll!SetBitmapBits",
4360: "gdi32.dll!NtGdiSetBitmapDimension",
4361: "gdi32.dll!NtGdiSetBoundsRect",
4362: "gdi32.dll!NtGdiSetBrushAttributes",
4363: "gdi32.dll!NtGdiSetBrushOrg",
4364: "gdi32.dll!NtGdiSetColorAdjustment",
4365: "gdi32.dll!NtGdiSetColorSpace",
4366: "gdi32.dll!NtGdiSetDeviceGammaRamp",
4367: "gdi32.dll!NtGdiSetDIBitsToDeviceInternal",
4368: "gdi32.dll!NtGdiSetFontEnumeration",
4369: "gdi32.dll!NtGdiSetFontXform",
4370: "gdi32.dll!NtGdiSetIcmMode",
4371: "gdi32.dll!NtGdiSetLinkedUFIs",
4372: "gdi32.dll!NtGdiSetMagicColors",
4373: "gdi32.dll!NtGdiSetMetaRgn",
4374: "gdi32.dll!NtGdiSetMiterLimit",
4375: "gdi32.dll!NtGdiGetDeviceWidth",
4376: "gdi32.dll!NtGdiMirrorWindowOrg",
4377: "gdi32.dll!NtGdiSetLayout",
4378: "gdi32.dll!NtGdiSetPixel",
4379: "gdi32.dll!NtGdiSetPixelFormat",
4380: "gdi32.dll!NtGdiSetRectRgn",
4381: "gdi32.dll!NtGdiSetSystemPaletteUse",
4383: "gdi32.dll!NtGdiSetupPublicCFONT",
4384: "gdi32.dll!NtGdiSetVirtualResolution",
4385: "gdi32.dll!NtGdiSetSizeDevice",
4386: "gdi32.dll!NtGdiStartDoc",
4387: "gdi32.dll!NtGdiStartPage",
4388: "gdi32.dll!NtGdiStretchBlt",
4389: "gdi32.dll!NtGdiStretchDIBitsInternal",
4390: "gdi32.dll!NtGdiStrokeAndFillPath",
4391: "gdi32.dll!NtGdiStrokePath",
4392: "gdi32.dll!NtGdiSwapBuffers",
4393: "gdi32.dll!NtGdiTransformPoints",
4394: "gdi32.dll!NtGdiTransparentBlt",
4395: "gdi32.dll!NtGdiUnloadPrinterDriver",
4397: "gdi32.dll!NtGdiUnrealizeObject",
4398: "gdi32.dll!NtGdiUpdateColors",
4399: "gdi32.dll!NtGdiWidenPath",
4400: "user32.dll!NtUserActivateKeyboardLayout",
4401: "user32.dll!NtUserAlterWindowStyle",
4402: "imm32.dll!NtUserAssociateInputContext",
4403: "user32.dll!NtUserAttachThreadInput",
4404: "user32.dll!NtUserBeginPaint",
4405: "user32.dll!NtUserBitBltSysBmp",
4406: "user32.dll!NtUserBlockInput",
4407: "imm32.dll!NtUserBuildHimcList",
4408: "user32.dll!NtUserBuildHwndList",
4409: "user32.dll!NtUserBuildNameList",
4410: "user32.dll!NtUserBuildPropList",
4411: "user32.dll!NtUserCallHwnd",
4412: "user32.dll!NtUserCallHwndLock",
4413: "user32.dll!NtUserCallHwndOpt",
4414: "user32.dll!NtUserCallHwndParam",
4415: "user32.dll!NtUserCallHwndParamLock",
4416: "user32.dll!NtUserCallMsgFilter",
4417: "user32.dll!NtUserCallNextHookEx",
4418: "user32.dll!NtUserCallNoParam",
4419: "imm32.dll!NtUserCallOneParam",
4419: "user32.dll!NtUserCallOneParam",
4420: "user32.dll!NtUserCallTwoParam",
4421: "user32.dll!NtUserChangeClipboardChain",
4422: "user32.dll!NtUserChangeDisplaySettings",
4424: "user32.dll!NtUserCheckMenuItem",
4425: "user32.dll!NtUserChildWindowFromPointEx",
4426: "user32.dll!NtUserClipCursor",
4427: "user32.dll!NtUserCloseClipboard",
4428: "user32.dll!NtUserCloseDesktop",
4429: "user32.dll!NtUserCloseWindowStation",
4431: "user32.dll!NtUserConvertMemHandle",
4432: "user32.dll!NtUserCopyAcceleratorTable",
4433: "user32.dll!NtUserCountClipboardFormats",
4434: "user32.dll!NtUserCreateAcceleratorTable",
4435: "user32.dll!NtUserCreateCaret",
4436: "user32.dll!NtUserCreateDesktop",
4437: "imm32.dll!NtUserCreateInputContext",
4438: "user32.dll!NtUserCreateLocalMemHandle",
4439: "user32.dll!NtUserCreateWindowEx",
4440: "user32.dll!NtUserCreateWindowStation",
4441: "user32.dll!NtUserDdeGetQualityOfService",
4442: "user32.dll!NtUserDdeInitialize",
4443: "user32.dll!NtUserDdeSetQualityOfService",
4444: "user32.dll!NtUserDeferWindowPos",
4445: "user32.dll!NtUserDefSetText",
4446: "user32.dll!NtUserDeleteMenu",
4447: "user32.dll!DestroyAcceleratorTable",
4448: "user32.dll!NtUserDestroyCursor",
4449: "imm32.dll!NtUserDestroyInputContext",
4450: "user32.dll!NtUserDestroyMenu",
4451: "user32.dll!NtUserDestroyWindow",
4452: "imm32.dll!NtUserDisableThreadIme",
4453: "user32.dll!NtUserDispatchMessage",
4454: "user32.dll!NtUserDragDetect",
4455: "user32.dll!NtUserDragObject",
4456: "user32.dll!NtUserDrawAnimatedRects",
4457: "user32.dll!NtUserDrawCaption",
4458: "user32.dll!NtUserDrawCaptionTemp",
4459: "user32.dll!NtUserDrawIconEx",
4460: "user32.dll!NtUserDrawMenuBarTemp",
4461: "user32.dll!NtUserEmptyClipboard",
4462: "user32.dll!NtUserEnableMenuItem",
4463: "user32.dll!NtUserEnableScrollBar",
4464: "user32.dll!NtUserEndDeferWindowPosEx",
4465: "user32.dll!NtUserEndMenu",
4466: "user32.dll!NtUserEndPaint",
4467: "user32.dll!NtUserEnumDisplayDevices",
4468: "user32.dll!NtUserEnumDisplayMonitors",
4469: "user32.dll!NtUserEnumDisplaySettings",
4470: "user32.dll!NtUserEvent",
4471: "user32.dll!NtUserExcludeUpdateRgn",
4472: "user32.dll!NtUserFillWindow",
4473: "user32.dll!NtUserFindExistingCursorIcon",
4474: "user32.dll!NtUserFindWindowEx",
4475: "user32.dll!NtUserFlashWindowEx",
4476: "user32.dll!NtUserGetAltTabInfo",
4477: "user32.dll!NtUserGetAncestor",
4478: "imm32.dll!NtUserGetAppImeLevel",
4479: "user32.dll!GetAsyncKeyState",
4480: "user32.dll!NtUserGetAtomName",
4481: "user32.dll!NtUserGetCaretBlinkTime",
4482: "user32.dll!NtUserGetCaretPos",
4483: "user32.dll!NtUserGetClassInfo",
4484: "user32.dll!NtUserGetClassName",
4485: "user32.dll!NtUserGetClipboardData",
4486: "user32.dll!NtUserGetClipboardFormatName",
4487: "user32.dll!NtUserGetClipboardOwner",
4488: "user32.dll!NtUserGetClipboardSequenceNumber",
4489: "user32.dll!NtUserGetClipboardViewer",
4490: "user32.dll!NtUserGetClipCursor",
4491: "user32.dll!NtUserGetComboBoxInfo",
4492: "user32.dll!NtUserGetControlBrush",
4493: "user32.dll!NtUserGetControlColor",
4494: "user32.dll!NtUserGetCPD",
4495: "user32.dll!NtUserGetCursorFrameInfo",
4496: "user32.dll!NtUserGetCursorInfo",
4497: "user32.dll!NtUserGetDC",
4498: "user32.dll!NtUserGetDCEx",
4499: "user32.dll!NtUserGetDoubleClickTime",
4500: "user32.dll!NtUserGetForegroundWindow",
4501: "user32.dll!NtUserGetGuiResources",
4502: "user32.dll!NtUserGetGUIThreadInfo",
4503: "user32.dll!NtUserGetIconInfo",
4504: "user32.dll!NtUserGetIconSize",
4505: "imm32.dll!NtUserGetImeHotKey",
4505: "user32.dll!NtUserGetImeHotKey",
4506: "imm32.dll!NtUserGetImeInfoEx",
4507: "user32.dll!NtUserGetInternalWindowPos",
4508: "user32.dll!NtUserGetKeyboardLayoutList",
4509: "user32.dll!NtUserGetKeyboardLayoutName",
4510: "user32.dll!NtUserGetKeyboardState",
4511: "user32.dll!NtUserGetKeyNameText",
4512: "user32.dll!NtUserGetKeyState",
4513: "user32.dll!NtUserGetListBoxInfo",
4514: "user32.dll!NtUserGetMenuBarInfo",
4515: "user32.dll!NtUserGetMenuIndex",
4516: "user32.dll!NtUserGetMenuItemRect",
4517: "user32.dll!NtUserGetMessage",
4518: "user32.dll!NtUserGetMouseMovePointsEx",
4519: "user32.dll!NtUserGetObjectInformation",
4520: "user32.dll!NtUserGetOpenClipboardWindow",
4521: "user32.dll!NtUserGetPriorityClipboardFormat",
4522: "user32.dll!NtUserGetProcessWindowStation",
4523: "user32.dll!NtUserGetRawInputBuffer",
4524: "user32.dll!NtUserGetRawInputData",
4525: "user32.dll!NtUserGetRawInputDeviceInfo",
4526: "user32.dll!NtUserGetRawInputDeviceList",
4527: "user32.dll!NtUserGetRegisteredRawInputDevices",
4528: "user32.dll!NtUserGetScrollBarInfo",
4529: "user32.dll!NtUserGetSystemMenu",
4530: "user32.dll!NtUserGetThreadDesktop",
4531: "imm32.dll!NtUserGetThreadState",
4531: "user32.dll!NtUserGetThreadState",
4532: "user32.dll!NtUserGetTitleBarInfo",
4533: "user32.dll!GetUpdateRect",
4534: "user32.dll!GetUpdateRgn",
4535: "user32.dll!NtUserGetWindowDC",
4536: "user32.dll!NtUserGetWindowPlacement",
4537: "user32.dll!NtUserGetWOWClass",
4539: "user32.dll!NtUserHideCaret",
4540: "user32.dll!NtUserHiliteMenuItem",
4541: "user32.dll!NtUserImpersonateDdeClientWindow",
4543: "user32.dll!NtUserInitializeClientPfnArrays",
4544: "user32.dll!NtUserInitTask",
4545: "user32.dll!NtUserInternalGetWindowText",
4546: "user32.dll!NtUserInvalidateRect",
4547: "user32.dll!NtUserInvalidateRgn",
4548: "user32.dll!NtUserIsClipboardFormatAvailable",
4549: "user32.dll!NtUserKillTimer",
4550: "user32.dll!NtUserLoadKeyboardLayoutEx",
4551: "user32.dll!NtUserLockWindowStation",
4552: "user32.dll!NtUserLockWindowUpdate",
4553: "user32.dll!NtUserLockWorkStation",
4554: "user32.dll!NtUserMapVirtualKeyEx",
4555: "user32.dll!NtUserMenuItemFromPoint",
4556: "user32.dll!NtUserMessageCall",
4557: "user32.dll!NtUserMinMaximize",
4558: "user32.dll!NtUserMNDragLeave",
4559: "user32.dll!NtUserMNDragOver",
4560: "user32.dll!NtUserModifyUserStartupInfoFlags",
4561: "user32.dll!NtUserMoveWindow",
4562: "imm32.dll!NtUserNotifyIMEStatus",
4562: "user32.dll!NtUserNotifyIMEStatus",
4564: "user32.dll!NtUserNotifyWinEvent",
4565: "user32.dll!NtUserOpenClipboard",
4566: "user32.dll!NtUserOpenDesktop",
4567: "user32.dll!NtUserOpenInputDesktop",
4568: "user32.dll!NtUserOpenWindowStation",
4569: "user32.dll!NtUserPaintDesktop",
4570: "user32.dll!PeekMessageW",
4571: "user32.dll!NtUserPostMessage",
4572: "user32.dll!NtUserPostThreadMessage",
4573: "user32.dll!NtUserPrintWindow",
4574: "user32.dll!NtUserProcessConnect",
4576: "imm32.dll!NtUserQueryInputContext",
4577: "user32.dll!NtUserQuerySendMessage",
4578: "user32.dll!NtUserQueryUserCounters",
4579: "imm32.dll!NtUserQueryWindow",
4579: "user32.dll!NtUserQueryWindow",
4580: "user32.dll!NtUserRealChildWindowFromPoint",
4581: "user32.dll!NtUserRealInternalGetMessage",
4582: "user32.dll!NtUserRealWaitMessageEx",
4583: "user32.dll!NtUserRedrawWindow",
4584: "user32.dll!NtUserRegisterClassExWOW",
4585: "user32.dll!NtUserRegisterUserApiHook",
4586: "user32.dll!NtUserRegisterHotKey",
4587: "user32.dll!NtUserRegisterRawInputDevices",
4588: "user32.dll!NtUserRegisterTasklist",
4589: "user32.dll!NtUserRegisterWindowMessage",
4590: "user32.dll!NtUserRemoveMenu",
4591: "user32.dll!NtUserRemoveProp",
4593: "user32.dll!NtUserResolveDesktopForWOW",
4594: "user32.dll!NtUserSBGetParms",
4595: "user32.dll!ScrollDC",
4596: "user32.dll!NtUserScrollWindowEx",
4597: "gdi32.dll!NtUserSelectPalette",
4598: "user32.dll!NtUserSendInput",
4599: "user32.dll!NtUserSetActiveWindow",
4600: "imm32.dll!NtUserSetAppImeLevel",
4601: "user32.dll!NtUserSetCapture",
4602: "user32.dll!NtUserSetClassLong",
4603: "user32.dll!NtUserSetClassWord",
4604: "user32.dll!NtUserSetClipboardData",
4605: "user32.dll!NtUserSetClipboardViewer",
4606: "user32.dll!NtUserSetConsoleReserveKeys",
4607: "user32.dll!NtUserSetCursor",
4608: "user32.dll!NtUserSetCursorContents",
4609: "user32.dll!NtUserSetCursorIconData",
4610: "user32.dll!NtUserSetDbgTag",
4611: "user32.dll!NtUserSetFocus",
4612: "user32.dll!NtUserSetImeHotKey",
4613: "imm32.dll!NtUserSetImeInfoEx",
4614: "user32.dll!NtUserSetImeOwnerWindow",
4616: "user32.dll!NtUserSetInformationThread",
4617: "user32.dll!NtUserSetInternalWindowPos",
4618: "user32.dll!NtUserSetKeyboardState",
4619: "user32.dll!NtUserSetLogonNotifyWindow",
4620: "user32.dll!NtUserSetMenu",
4621: "user32.dll!NtUserSetMenuContextHelpId",
4622: "user32.dll!NtUserSetMenuDefaultItem",
4623: "user32.dll!NtUserSetMenuFlagRtoL",
4624: "user32.dll!NtUserSetObjectInformation",
4625: "user32.dll!NtUserSetParent",
4626: "user32.dll!NtUserSetProcessWindowStation",
4627: "user32.dll!NtUserSetProp",
4628: "user32.dll!NtUserSetRipFlags",
4629: "user32.dll!NtUserSetScrollInfo",
4630: "user32.dll!NtUserSetShellWindowEx",
4631: "user32.dll!NtUserSetSysColors",
4632: "user32.dll!NtUserSetSystemCursor",
4633: "user32.dll!NtUserSetSystemMenu",
4634: "user32.dll!NtUserSetSystemTimer",
4635: "user32.dll!NtUserSetThreadDesktop",
4636: "imm32.dll!NtUserSetThreadLayoutHandles",
4637: "user32.dll!NtUserSetThreadState",
4638: "user32.dll!NtUserSetTimer",
4639: "user32.dll!NtUserSetWindowFNID",
4640: "user32.dll!NtUserSetWindowLong",
4641: "user32.dll!NtUserSetWindowPlacement",
4642: "user32.dll!NtUserSetWindowPos",
4643: "user32.dll!NtUserSetWindowRgn",
4644: "user32.dll!NtUserSetWindowsHookAW",
4645: "user32.dll!NtUserSetWindowsHookEx",
4646: "user32.dll!NtUserSetWindowStationUser",
4647: "user32.dll!NtUserSetWindowWord",
4648: "user32.dll!NtUserSetWinEventHook",
4649: "user32.dll!NtUserShowCaret",
4650: "user32.dll!NtUserShowScrollBar",
4651: "user32.dll!NtUserShowWindow",
4652: "user32.dll!NtUserShowWindowAsync",
4654: "user32.dll!NtUserSwitchDesktop",
4655: "user32.dll!NtUserSystemParametersInfo",
4657: "user32.dll!NtUserThunkedMenuInfo",
4658: "user32.dll!NtUserThunkedMenuItemInfo",
4659: "user32.dll!NtUserToUnicodeEx",
4660: "user32.dll!NtUserTrackMouseEvent",
4661: "user32.dll!NtUserTrackPopupMenuEx",
4662: "user32.dll!NtUserCalcMenuBar",
4663: "user32.dll!NtUserPaintMenuBar",
4664: "user32.dll!TranslateAcceleratorA",
4665: "user32.dll!NtUserTranslateMessage",
4666: "user32.dll!NtUserUnhookWindowsHookEx",
4667: "user32.dll!NtUserUnhookWinEvent",
4668: "user32.dll!NtUserUnloadKeyboardLayout",
4669: "user32.dll!NtUserUnlockWindowStation",
4670: "user32.dll!NtUserUnregisterClass",
4671: "user32.dll!NtUserUnregisterUserApiHook",
4672: "user32.dll!NtUserUnregisterHotKey",
4673: "imm32.dll!NtUserUpdateInputContext",
4673: "user32.dll!NtUserUpdateInputContext",
4674: "user32.dll!NtUserUpdateInstance",
4675: "user32.dll!NtUserUpdateLayeredWindow",
4676: "user32.dll!NtUserGetLayeredWindowAttributes",
4677: "user32.dll!NtUserSetLayeredWindowAttributes",
4678: "user32.dll!NtUserUpdatePerUserSystemParameters",
4679: "user32.dll!NtUserUserHandleGrantAccess",
4680: "imm32.dll!NtUserValidateHandleSecure",
4680: "user32.dll!NtUserValidateHandleSecure",
4681: "user32.dll!NtUserValidateRect",
4682: "user32.dll!NtUserValidateTimerCallback",
4683: "user32.dll!NtUserVkKeyScanEx",
4684: "user32.dll!NtUserWaitForInputIdle",
4685: "user32.dll!NtUserWaitForMsgAndEvent",
4686: "user32.dll!NtUserWaitMessage",
4687: "user32.dll!NtUserWin32PoolAllocationStats",
4688: "user32.dll!NtUserWindowFromPoint",
4689: "user32.dll!NtUserYieldTask",
4695: "gdi32.dll!NtGdiEngAssociateSurface",
4696: "gdi32.dll!NtGdiEngCreateBitmap",
4697: "gdi32.dll!NtGdiEngCreateDeviceSurface",
4698: "gdi32.dll!NtGdiEngCreateDeviceBitmap",
4699: "gdi32.dll!NtGdiEngCreatePalette",
4700: "gdi32.dll!NtGdiEngComputeGlyphSet",
4701: "gdi32.dll!NtGdiEngCopyBits",
4702: "gdi32.dll!NtGdiEngDeletePalette",
4703: "gdi32.dll!NtGdiEngDeleteSurface",
4704: "gdi32.dll!NtGdiEngEraseSurface",
4705: "gdi32.dll!NtGdiEngUnlockSurface",
4706: "gdi32.dll!NtGdiEngLockSurface",
4707: "gdi32.dll!NtGdiEngBitBlt",
4708: "gdi32.dll!NtGdiEngStretchBlt",
4709: "gdi32.dll!NtGdiEngPlgBlt",
4710: "gdi32.dll!NtGdiEngMarkBandingSurface",
4711: "gdi32.dll!NtGdiEngStrokePath",
4712: "gdi32.dll!NtGdiEngFillPath",
4713: "gdi32.dll!NtGdiEngStrokeAndFillPath",
4714: "gdi32.dll!NtGdiEngPaint",
4715: "gdi32.dll!NtGdiEngLineTo",
4716: "gdi32.dll!NtGdiEngAlphaBlend",
4717: "gdi32.dll!NtGdiEngGradientFill",
4718: "gdi32.dll!NtGdiEngTransparentBlt",
4719: "gdi32.dll!NtGdiEngTextOut",
4720: "gdi32.dll!NtGdiEngStretchBltROP",
4721: "gdi32.dll!NtGdiXLATEOBJ_cGetPalette",
4722: "gdi32.dll!NtGdiXLATEOBJ_iXlate",
4723: "gdi32.dll!NtGdiXLATEOBJ_hGetColorTransform",
4724: "gdi32.dll!NtGdiCLIPOBJ_bEnum",
4725: "gdi32.dll!NtGdiCLIPOBJ_cEnumStart",
4726: "gdi32.dll!NtGdiCLIPOBJ_ppoGetPath",
4727: "gdi32.dll!NtGdiEngDeletePath",
4728: "gdi32.dll!NtGdiEngCreateClip",
4729: "gdi32.dll!NtGdiEngDeleteClip",
4730: "gdi32.dll!NtGdiBRUSHOBJ_ulGetBrushColor",
4731: "gdi32.dll!NtGdiBRUSHOBJ_pvAllocRbrush",
4732: "gdi32.dll!NtGdiBRUSHOBJ_pvGetRbrush",
4733: "gdi32.dll!NtGdiBRUSHOBJ_hGetColorTransform",
4734: "gdi32.dll!NtGdiXFORMOBJ_bApplyXform",
4735: "gdi32.dll!NtGdiXFORMOBJ_iGetXform",
4736: "gdi32.dll!NtGdiFONTOBJ_vGetInfo",
4737: "gdi32.dll!NtGdiFONTOBJ_pxoGetXform",
4738: "gdi32.dll!NtGdiFONTOBJ_cGetGlyphs",
4739: "gdi32.dll!NtGdiFONTOBJ_pifi",
4740: "gdi32.dll!NtGdiFONTOBJ_pfdg",
4741: "gdi32.dll!NtGdiFONTOBJ_pQueryGlyphAttrs",
4742: "gdi32.dll!NtGdiFONTOBJ_pvTrueTypeFontFile",
4743: "gdi32.dll!NtGdiFONTOBJ_cGetAllGlyphHandles",
4744: "gdi32.dll!NtGdiSTROBJ_bEnum",
4745: "gdi32.dll!NtGdiSTROBJ_bEnumPositionsOnly",
4746: "gdi32.dll!NtGdiSTROBJ_bGetAdvanceWidths",
4747: "gdi32.dll!NtGdiSTROBJ_vEnumStart",
4748: "gdi32.dll!NtGdiSTROBJ_dwGetCodePage",
4749: "gdi32.dll!NtGdiPATHOBJ_vGetBounds",
4750: "gdi32.dll!NtGdiPATHOBJ_bEnum",
4751: "gdi32.dll!NtGdiPATHOBJ_vEnumStart",
4752: "gdi32.dll!NtGdiPATHOBJ_vEnumStartClipLines",
4753: "gdi32.dll!NtGdiPATHOBJ_bEnumClipLines",
4754: "gdi32.dll!NtGdiGetDhpdev",
4755: "gdi32.dll!NtGdiEngCheckAbort",
4756: "gdi32.dll!NtGdiHT_Get8BPPFormatPalette",
4757: "gdi32.dll!NtGdiHT_Get8BPPMaskPalette",
4758: "gdi32.dll!NtGdiUpdateTransform",
4759: "gdi32.dll!NtGdiSetPUMPDOBJ",
4760: "gdi32.dll!NtGdiBRUSHOBJ_DeleteRbrush",
4761: "gdi32.dll!NtGdiUMPDEngFreeUserMem",
4762: "gdi32.dll!NtGdiDrawStream",
}
