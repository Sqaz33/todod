window.addEventListener("load", function () {
  window.ui = SwaggerUIBundle({
    url: new URL("/openapi.json", window.location.origin).href,
    dom_id: "#swagger-ui",
    deepLinking: true,
    displayRequestDuration: true,
    filter: true,
    showExtensions: true,
    showCommonExtensions: true,
    tryItOutEnabled: false,
    validatorUrl: null,
    requestInterceptor: function (request) {
      request.url = new URL(request.url, window.location.origin).href;
      return request;
    },
    presets: [SwaggerUIBundle.presets.apis],
    syntaxHighlight: {activate: true, theme: "agate"}
  });
});
